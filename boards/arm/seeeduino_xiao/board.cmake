# Copyright (c) 2020 Google LLC.
# SPDX-License-Identifier: Apache-2.0

board_runner_args(bossac "--offset=0x2000")

include(${ZEPHYR_BASE}/boards/common/bossac.board.cmake)
include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
