files = files(
'mat4.h',
'mat4.c',
'screenshot-util.h',
'screenshot-util.c',
'shader.h',
'shader.c',
)

glfw3_dep = dependency('glfw3', default_options: ['install=true'])
fontconfig_dep = dependency('fontconfig')

executable('markup', files + ['markup.c'],
dependencies: [freetype_gl_dep, glfw3_dep, fontconfig_dep],
install: true)
