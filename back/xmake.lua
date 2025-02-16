add_rules("mode.debug", "mode.release")

-- apt install libmysqlcppconn-dev

set_languages("c++17")

add_requires("cpp-httplib", "nlohmann_json")

target("back")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("cpp-httplib", "nlohmann_json")
    add_links("mysqlcppconn")
    set_rundir(".")