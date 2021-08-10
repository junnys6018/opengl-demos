set(SOURCES
	stb_image.cpp
	stb_image.h
	stb_image_write.cpp
	stb_image_write.h
)

add_library(stb_image STATIC ${SOURCES})