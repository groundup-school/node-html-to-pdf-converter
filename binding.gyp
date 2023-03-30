{
	"targets": [
		{			
		    "target_name": "wkhtmltopdf_addon",
			"sources": [ "native_files/napi_wkhtmltopdf_binding.cpp" ],
		    "cflags!": [ "-fno-exceptions" ],
		    "cflags_cc!": [ "-fno-exceptions" ],
		    "include_dirs": [
		        "<!@(node -p \"require('node-addon-api').include\")"
		    ],
		    "defines": [ "NAPI_DISABLE_CPP_EXCEPTIONS" ],
		    "libraries": [
			    "-lwkhtmltox"
			]
		}
	]
}