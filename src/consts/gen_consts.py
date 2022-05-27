from absl import app
from absl import flags

import io
import yaml

FLAGS = flags.FLAGS

flags.DEFINE_string("yaml_path", "./consts.yaml", "Path to consts yaml file.")
flags.DEFINE_string("c_out", "", "Output c file.")
flags.DEFINE_string("go_out", "", "Output go file.")


def main(argv):
  del argv

  consts = load_yaml(FLAGS.yaml_path)

  if FLAGS.c_out:
    generate_c(consts, FLAGS.c_out)
  if FLAGS.go_out:
    generate_go(consts, FLAGS.go_out)


def load_yaml(yaml_path):
  with open(yaml_path, "r") as file:
    return yaml.load(file.read(), Loader=yaml.CLoader)


def generate_c(consts, out_path):
  builder = io.StringIO()
  builder.write("#ifndef HENRIETTA_SRC_CONSTS_H_\n")
  builder.write("#define HENRIETTA_SRC_CONSTS_H_\n")
  builder.write("\n")
  for key, val in consts.items():
    val_string = str(val)
    if type(val) is str:
      val_string = "\"{}\"".format(val_string)
    builder.write("#define {} {}\n".format(key.upper(), val_string))
  builder.write("\n")
  builder.write("#endif // HENRIETTA_SRC_CONSTS_H_\n")

  with open(out_path, "w") as file:
    file.write(builder.getvalue())


def generate_go(consts, out_path):
  builder = io.StringIO()
  builder.write("package consts\n")
  builder.write("\n")
  builder.write("const (\n")
  for key, val in consts.items():
    words = key.split("_")
    words = [word.capitalize() for word in words]
    val_string = str(val)
    if type(val) is str:
      val_string = "\"{}\"".format(val_string)
    builder.write("\t{} = {}\n".format("".join(words), val_string))
  builder.write(")\n")

  with open(out_path, "w") as file:
    file.write(builder.getvalue())


if __name__ == "__main__":
  app.run(main)
