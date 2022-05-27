load("@platformio_rules//platformio:platformio.bzl", "platformio_library")

# This is gnarly and hacky but it works, at least with the simple proto I tried.
def platformio_proto_library(name, proto):
    platformio_proto_name = proto.removesuffix(".proto") + "_platformio_proto"

    native.genrule(
        name = name + "_gen_nanopb",
        srcs = [proto],
        outs = [platformio_proto_name + ".pb.h", platformio_proto_name + ".pb.c"],
        # Featured hacks:
        # * CDing into the .proto file directory before protoc-ing means the
        #   generated files don't include relative directories, which platformio
        #   doesn't want.
        # * The platformio_library rule always replaces file extensions with .h
        #   and .cpp, so we use sed to change the include in the generated .pb.c
        #   file accordingly.
        cmd = """
            export SANDBOX_ROOT=`pwd`;
            cd `dirname $<`;
            cp `basename $<` """ + platformio_proto_name + """.proto;
            protoc --nanopb_out=$$SANDBOX_ROOT/$(RULEDIR) """ + platformio_proto_name + """.proto;
            cd $$SANDBOX_ROOT/$(RULEDIR);
            sed -i 's/""" + platformio_proto_name + ".pb.h/" + platformio_proto_name + ".h/' " + platformio_proto_name + ".pb.c",
    )

    platformio_library(
        name = name,
        hdr = platformio_proto_name + ".pb.h",
        src = platformio_proto_name + ".pb.c",
    )
