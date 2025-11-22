#!/usr/bin/env python3
"""Python port of compile.sh with file copy for compile_commands.json."""

from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
from pathlib import Path


def run_cmake(args: list[str]) -> None:
    """Run a cmake command and surface failures immediately."""
    subprocess.run(["cmake", *args], check=True)


def update_compile_commands(source: Path, destination: Path) -> None:
    """Copy compile_commands.json if it exists, otherwise warn."""
    destination.parent.mkdir(parents=True, exist_ok=True)
    if source.exists():
        shutil.copy2(source, destination)
        print(f"==> 已更新 {destination} (来源: {source})")
    else:
        print(f"警告: 未找到 {source}，跳过 compile_commands.json 更新。")


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="CMake preset build helper.")
    parser.add_argument(
        "build_type",
        choices=("debug", "release"),
        help="选择 debug 或 release",
    )
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    preset = args.build_type.lower()

    print(f"==> 配置 ({preset})")
    run_cmake(["--preset", preset])

    compile_db = Path("build") / preset / "compile_commands.json"
    shared_compile_db = Path("build") / "compile_commands.json"
    update_compile_commands(compile_db, shared_compile_db)

    print(f"==> 构建 ({preset})")
    run_cmake(["--build", "--preset", preset])
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
