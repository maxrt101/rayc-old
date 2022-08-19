#!/usr/bin/env python3

from build.config import format as cf
import build, os, shutil

@build.config.feature_handler
def features(profile, feature_list):
    if profile == 'debug':
        build.config.get('cpp', 'cxxflags').extend(['-g3', '-D_DEBUG'])
        # if 'MEM' in feature_list: build.config.get('cpp', 'cxxflags').append('-D_FF_MEMORY_DEBUG')

@build.task()
def install_headers(ctx):
    src, dst = cf('{topdir}/include'), cf('{build_dir}/{profile}/include/rayc')
    if os.path.exists(dst):
        shutil.rmtree(dst)
    os.mkdir(dst)
    build.utils.copytree(src, dst)

@build.task(['librayc'])
def rayc(ctx):
    build.cpp.compile(cf('{topdir}/src/rayc.cc'))
    build.cpp.link_exe(
       files=[cf('{build_dir}/{profile}/obj/rayc.o')],
       output='rayc',
       libs=['rayc', 'sdl2', 'sdl2_image', 'sdl2_ttf']
    )

@build.task(['librayc'])
def map_tool(ctx):
    build.cpp.compile(cf('{topdir}/src/map_tool.cc'))
    build.cpp.link_exe(
       files=[cf('{build_dir}/{profile}/obj/map_tool.o')],
       output='map_tool',
       libs=['rayc', 'sdl2']
    )

@build.task(['install_headers'])
def librayc(ctx):
    build.cpp.compile_batch([
        cf('{topdir}/src/app.cc'),
        cf('{topdir}/src/log.cc'),
        cf('{topdir}/src/map.cc'),
        cf('{topdir}/src/data.cc'),
        cf('{topdir}/src/object.cc'),
        cf('{topdir}/src/config.cc'),
        cf('{topdir}/src/intutils.cc'),
        cf('{topdir}/src/strutils.cc'),
        cf('{topdir}/src/math/rect.cc'),
        cf('{topdir}/src/video/draw.cc'),
        cf('{topdir}/src/video/font.cc'),
        cf('{topdir}/src/video/texture.cc')
    ], 'rayc')
    build.cpp.create_static_lib(
        files=build.cpp.get_objs([
            cf('{build_dir}/{profile}/obj/rayc')
        ]),
        output='librayc.a'
    )

@build.task()
def rayc_map(ctx):
    pass

build.cli.run('rayc', 'build.json')
