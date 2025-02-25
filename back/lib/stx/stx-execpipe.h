// -*- mode: c++; fill-column: 79 -*-

/*
 * STX Execution Pipe Library v0.7.0
 * Copyright (C) 2010 Timo Bingmann
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _STX_EXECPIPE_H_
#define _STX_EXECPIPE_H_

#include <string>
#include <vector>

/// STX - Some Template Extensions namespace
namespace stx {

/**
 * Abstract class used as an input stream source for an ExecPipe.
 *
 * Derived classes can be used in an ExecPipe to generate an input stream
 * source. Data generated by this class is written to the first stage of the
 * pipe.
 *
 * When data is needed by the pipe the function poll() is called. This pure
 * virtual function must generate data and push it into a buffer using the
 * write() function. The input stream is terminated when poll() returns false.
 */
class PipeSource {
  private:
	/// pointer to associated pipe. filled by ExecPipe::add_input_source()
	class ExecPipeImpl* m_impl;

	/// association to the pipe implementation for write access to m_impl.
	friend class ExecPipeImpl;

  public:
	/// Constructor which clears m_impl.
	PipeSource();

	/// Poll the input source for new data. The input stream is closed when
	/// this function returns false, otherwise it will be polled again.
	virtual bool poll() = 0;

	/// Write input data to the first stage via a buffer.
	void write(const void* data, unsigned int datalen);
};

/**
 * Abstract class used as an output stream source for an ExecPipe.
 *
 * Derived classes can be attached to the end of an execution pipe. It will
 * receive all data outputted by the final pipe stage.
 *
 * Data read from the final or preceding stage is passed to the class via
 * process(). When the final stage closes the pipe, the function eof() is
 * called.
 */
class PipeSink {
  public:
	/// Pure virtual function which receives the output stream as read from the
	/// final or preceding pipe stage.
	virtual void process(const void* data, unsigned int datalen) = 0;

	/// Pure virtual function called when the final or preceding pipe stage
	/// finishes.
	virtual void eof() = 0;
};

/**
 * Abstract class used as an intermediate pipe stage between executed
 * processes.
 *
 * Derived classes can be inserted into an execution pipe between two
 * externally executed processes. It will receive all data from the preceding
 * pipe stage and after processing it may forward output to the next pipe
 * stage.
 *
 * The class is derived from PipeSink and receives data from the preceding
 * stage via the inherited functions process() and also the eof()
 * signal. Usually process() will perform some action on the data and then
 * forward the resulting data block to the next pipe stage via write().
 */
class PipeFunction : public PipeSink {
  private:
	/// pointer to associated pipe filled by ExecPipe::add_function()
	class ExecPipeImpl* m_impl;

	/// pipe stage identifier
	unsigned int m_stageid;

	/// association to the pipe implementation for write access to m_impl.
	friend class ExecPipeImpl;

  public:
	/// Constructor which clears m_impl and m_stageid.
	PipeFunction();

	/// Write input data to the next pipe stage via a buffer.
	void write(const void* data, unsigned int datalen);
};

/**
 * \brief Main library interface (reference counted pointer)
 *
 * The ExecPipe class is the main interface to the library. It is a reference
 * counted pointer implementation, so you can easily copy and pass around
 * without duplicating the inside object. See the \ref index "main page" for
 * detailed information and examples.
 */
class ExecPipe {
  protected:
	/// reference-counted pointer implementation
	class ExecPipeImpl* m_impl;

  public:
	/// Construct a new uninitialize execution pipe.
	ExecPipe();

	/// Release reference to execution pipe.
	~ExecPipe();

	/// Copy-constructor creates a new reference to the _same_ pipe.
	ExecPipe(const ExecPipe& ep);

	/// Assignment operator creates a new reference to the right pipe.
	ExecPipe& operator=(const ExecPipe& ep);

	// *** Debugging Output ***

	/// Enumeration for ascending debug levels.
	enum DebugLevel {
		DL_ERROR = 0,  ///< error reporting is always active. shows failed syscalls.
		DL_INFO = 1,   ///< info reports at important points during pipe run.
		DL_DEBUG = 2,  ///< debug shows information about select() calls.
		DL_TRACE = 3   ///< trace lists lots of info about read() and write() calls.
	};

	/// Change the current debug level. The default is DL_ERROR.
	void set_debug_level(enum DebugLevel dl);

	/// Change output function for debug messages. If set to NULL (the default)
	/// the debug lines are printed to stdout.
	void set_debug_output(void (*output)(const char* line));

	// *** Input Selectors ***

	///@{ \name Input Selectors

	/**
	 * Assign an already opened file descriptor as input stream for the first
	 * exec stage.
	 */
	void set_input_fd(int fd);

	/**
	 * Assign a file as input stream source. This file will be opened read-only
	 * and read by the first exec stage.
	 */
	void set_input_file(const char* path);

	/**
	 * Assign a std::string as input stream source. The contents of the string
	 * will be written to the first exec stage. The string object is not copied
	 * and must still exist when run() is called.
	 */
	void set_input_string(const std::string* input);

	/**
	 * Assign a PipeSource as input stream source. The object will be queried
	 * via the read() function for data which is then written to the first exec
	 * stage.
	 */
	void set_input_source(PipeSource* source);

	///@}

	// *** Output Selectors ***

	///@{ \name Output Selectors

	/**
	 * Assign an already opened file descriptor as output stream for the last
	 * exec stage.
	 */
	void set_output_fd(int fd);

	/**
	 * Assign a file as output stream destination. This file will be created or
	 * truncated write-only and written by the last exec stage.
	 */
	void set_output_file(const char* path, int mode = 0666);

	/**
	 * Assign a std::string as output stream destination. The output of the
	 * last exec stage will be stored as the contents of the string. The string
	 * object is not copied and must still exist when run() is called.
	 */
	void set_output_string(std::string* output);

	/**
	 * Assign a PipeSink as output stream destination. The object will receive
	 * data via the process() function and is informed via eof()
	 */
	void set_output_sink(PipeSink* sink);

	///@}

	// *** Pipe Stages ***

	///@{ \name Add Pipe Stages

	/**
	 * Return the number of pipe stages added.
	 */
	unsigned int size() const;

	/**
	 * Add an exec() stage to the pipe with given arguments. Note that argv[0]
	 * is set to prog.
	 */
	void add_exec(const char* prog);

	/**
	 * Add an exec() stage to the pipe with given arguments. Note that argv[0]
	 * is set to prog.
	 */
	void add_exec(const char* prog, const char* arg1);

	/**
	 * Add an exec() stage to the pipe with given arguments. Note that argv[0]
	 * is set to prog.
	 */
	void add_exec(const char* prog, const char* arg1, const char* arg2);

	/**
	 * Add an exec() stage to the pipe with given arguments. Note that argv[0]
	 * is set to prog.
	 */
	void add_exec(const char* prog, const char* arg1, const char* arg2, const char* arg3);

	/**
	 * Add an exec() stage to the pipe with given arguments. The vector of
	 * arguments is not copied, so it must still exist when run() is
	 * called. Note that the program called is args[0].
	 */
	void add_exec(const std::vector<std::string>* args);


	/**
	 * Add an execp() stage to the pipe with given arguments. The PATH variable
	 * is search for programs not containing a slash / character. Note that
	 * argv[0] is set to prog.
	 */
	void add_execp(const char* prog);

	/**
	 * Add an execp() stage to the pipe with given arguments. The PATH variable
	 * is search for programs not containing a slash / character. Note that
	 * argv[0] is set to prog.
	 */
	void add_execp(const char* prog, const char* arg1);

	/**
	 * Add an execp() stage to the pipe with given arguments. The PATH variable
	 * is search for programs not containing a slash / character. Note that
	 * argv[0] is set to prog.
	 */
	void add_execp(const char* prog, const char* arg1, const char* arg2);

	/**
	 * Add an execp() stage to the pipe with given arguments. The PATH variable
	 * is search for programs not containing a slash / character. Note that
	 * argv[0] is set to prog.
	 */
	void add_execp(const char* prog, const char* arg1, const char* arg2, const char* arg3);

	/**
	 * Add an execp() stage to the pipe with given arguments. The PATH variable
	 * is search for programs not containing a slash / character. The vector of
	 * arguments is not copied, so it must still exist when run() is
	 * called. Note that the program called is args[0].
	 */
	void add_execp(const std::vector<std::string>* args);

	/**
	 * Add an exece() stage to the pipe with the given arguments and
	 * environments. This is the most flexible exec() call. The vector of
	 * arguments and environment variables is not copied, so it must still
	 * exist when run() is called. The args[0] is _not_ override with path, so
	 * you can fake program name calls.
	 */
	void add_exece(const char* path, const std::vector<std::string>* args, const std::vector<std::string>* env);

	/**
	 * Add a function stage to the pipe. This function object will be called in
	 * the parent process with data passing through the stage. See PipeFunction
	 * for more information.
	 */
	void add_function(PipeFunction* func);

	///@}

	// *** Run Pipe ***

	/**
	 * Run the configured pipe sequence and wait for all children processes to
	 * complete. Returns a reference to *this for chaining.
	 *
	 * This function call should be wrapped into a try-catch block as it will
	 * throw() if a system call fails.
	 */
	ExecPipe& run();

	// *** Inspection After Pipe Execution ***

	///@{ \name Inspect Return Codes

	/**
	 * Get the return status of exec() stage's program run after pipe execution
	 * as indicated by wait().
	 */
	int get_return_status(unsigned int stageid) const;

	/**
	 * Get the return code of exec() stage's program run after pipe execution,
	 * or -1 if the program terminated abnormally.
	 */
	int get_return_code(unsigned int stageid) const;

	/**
	 * Get the signal of the abnormally terminated exec() stage's program run
	 * after pipe execution, or -1 if the program terminated normally.
	 */
	int get_return_signal(unsigned int stageid) const;

	/**
	 * Return true if the return code of all exec() stages were zero.
	 */
	bool all_return_codes_zero() const;

	///@}
};

}  // namespace stx

#endif	// _STX_EXECPIPE_H_