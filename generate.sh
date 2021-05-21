#!/bin/bash

set -ex

bazel_dir="bazel-${PWD##*/}"

find $bazel_dir/external  src extensions  -name '*.cc' > sourcefiles.txt

