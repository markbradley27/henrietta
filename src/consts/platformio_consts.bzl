load("@platformio_rules//platformio:platformio.bzl", "platformio_library")

def platformio_consts(name):
  native.genrule(
      name = name + "_gen_c",
      srcs = ["consts.yaml"],
      outs = ["platformio_consts.h"],
      cmd = "python3 $(location :gen_consts.py) --yaml_path $< --c_out $@",
      tools = ["gen_consts.py"],
  )

  platformio_library(
      name = name,
      hdr = "platformio_consts.h",
  )
