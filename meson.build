project('freetype-gl', 'c')

c = meson.get_compiler('c')
freetype2 = dependency('freetype2')
gl = dependency('opengl')
glew = dependency('glew')

conf_data = configuration_data()
configure_file(output: 'config.h', configuration: conf_data)
# TODO add config data

freetype_gl_sources = files('distance-field.c', 
                'edtaa3func.c', 
                'ftgl-utils.c',
                'font-manager.c', 
                'platform.c', 
                'text-buffer.c', 
                'texture-atlas.c', 
                'texture-font.c', 
                'utf8-utils.c',
                'vector.c',
                'vertex-attribute.c',
                'vertex-buffer.c')

inc = include_directories('.')
deps = [freetype2, gl, glew]
if host_machine.system() != 'windows'
    m = c.find_library('m')
    deps += m
endif

freetype_gl_lib = static_library('freetype-gl', freetype_gl_sources, include_directories: inc, dependencies: deps)
freetype_gl_dep = declare_dependency(include_directories: inc, dependencies: deps, link_with: freetype_gl_lib)

makefont_sources = files('makefont.c')
executable('makefont', makefont_sources, dependencies: freetype_gl_dep, install: true)
