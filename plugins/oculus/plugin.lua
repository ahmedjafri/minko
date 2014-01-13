--[[
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
]]--

-- oculus plugin
minko.plugin.oculus = {}

-- Note: if you see errors like this one:
-- /System/Library/Frameworks/IOKit.framework/Headers/usb/USB.h:754:23: error: too many #pragma options align=reset
-- please replace all occurences of #pragma options align=reset with:
-- #if defined(__GNUC__)
-- #pragma pack()
-- #else
-- #pragma options align=reset
-- #endif

function minko.plugin.oculus:enable()
	configuration { "windows or macosx" }
		defines { "MINKO_PLUGIN_OCULUS" }

		minko.plugin.links { "oculus" }

		includedirs {
			minko.sdk.path("plugins/oculus/include")
		}

	configuration { "win" }
		links { "winmm", "setupapi" }

		postbuildcommands {
			'xcopy /y /i /s "' .. minko.sdk.path('/plugins/oculus/asset/effect/*') .. '" "$(TargetDir)\\effect"',
		}

	configuration { "osx" }
		includedirs { 
			minko.sdk.path("plugins/oculus/include")
		}

		postbuildcommands {
			'cp -r ' .. minko.sdk.path('/plugins/oculus/asset/effect') .. ' ${TARGETDIR}',
		}
end