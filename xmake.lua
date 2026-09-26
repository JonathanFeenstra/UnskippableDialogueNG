-- include subprojects
includes("lib/commonlibsse-ng")

-- set project constants
set_project("Unskippable Dialogue NG")
set_version("2.0.1")
set_license("GPL-3.0-or-later WITH Modding Exception AND GPL-3.0 Linking Exception (with Corresponding Source)")
set_languages("c++23")
set_warnings("allextra")

-- add common rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- define targets
target("UnskippableDialogueNG")
    add_rules("commonlibsse-ng.plugin", {
        name = "UnskippableDialogueNG",
        author = "Jonathan Feenstra",
        description = "SKSE plugin that prevents you from skipping any dialogue."
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/PCH.h")
    
	-- add extra files
	add_extrafiles(".clang-format")