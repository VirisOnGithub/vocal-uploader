add_rules("mode.debug", "mode.release")

-- apt install libmysqlcppconn-dev

add_requires("cpp-httplib")

target("back")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("cpp-httplib")
    add_links("mysqlcppconn")
    set_rundir(".")