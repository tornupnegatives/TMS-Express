// Copyright (C) 2022-2024 Joseph Bellahcen <joeclb@icloud.com>

#include "ui/cli/CommandLineApp.hpp"

int main(int argc, char** argv) {
    auto cli = tms_express::ui::CommandLineApp();
    CLI11_PARSE(cli, argc, argv);
    return 0;
}
