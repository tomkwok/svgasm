#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

#define BUFFER_LEN 1024

#define DELAY_SECONDS 1
#define STDOUT_NAME "-"
#define ID_PREFIX "_"
#define CLEANER_CMD "svgcleaner -c %s"

#define TRANSITION_PERCENT 0.001

#define HELP_CONTENT "svgasm [-d delayseconds] [-o outfilepath] [-p idprefix] [-c cleanercmd] infilepath..."

inline std::string exec (const char* cmd) {
    std::cerr << cmd << std::endl;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        std::cerr << "Command execution failed." << std::endl;
        exit(0);
    }
    std::array<char, BUFFER_LEN> buffer;
    std::string result;
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

inline void string_replace (std::string& s, const std::string& search,
                    const std::string& replace) {
    size_t pos = 0;
    while ((pos = s.find(search, pos)) != std::string::npos) {
        s.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

int main (int argc, char *argv[]) {
    double delayseconds = DELAY_SECONDS;
    char outfilepath[BUFFER_LEN] = STDOUT_NAME;
    char idprefix[BUFFER_LEN] = ID_PREFIX;
    char cleanercmd[BUFFER_LEN] = CLEANER_CMD;
    int c;
    while ((c = getopt(argc, argv, "o:d:c:h")) != -1) {
        switch (c) {
            case 'd':
                delayseconds = atof(optarg);
                break;
            case 'o':
                std::strncpy(outfilepath, optarg, BUFFER_LEN);
                break;
            case 'p':
                std::strncpy(idprefix, optarg, BUFFER_LEN);
                break;
            case 'c':
                std::strncpy(cleanercmd, optarg, BUFFER_LEN);
                break;
            case 'h':
            default:
                std::cout << HELP_CONTENT << std::endl;
                exit(0);
                break;
        }
    }

    if (argc - optind < 2) {
        std::cout << HELP_CONTENT << std::endl;
        exit(0);
    }

    std::ostream* out;
    std::ofstream outfile;
    if (std::strcmp(outfilepath, STDOUT_NAME) == 0) {
        out = &std::cout;
    } else {
        outfile.open(outfilepath);
        if (!outfile.is_open()) {
            std::cerr << "Output file creation failed." << std::endl;
            exit(0);
        }
        out = &outfile;
    }

    for (int i = optind; i < argc; i++) {
        int idx = i - optind;

        /* run `svgcleaner` on input file to obtain a clean optimized file 
            without unnecessary white spaces and declarations before `<svg>` tag */
        char cmd[BUFFER_LEN] = "";
        std::snprintf(cmd, BUFFER_LEN, cleanercmd, argv[i]);
        std::string s = exec(cmd);

        /* find the first occurrence of `>` in string
            that should belong to the opening `<svg>` tag */
        size_t pos_start = s.find('>') + 1;

        if (idx == 0) {
            /* output opening `<svg>` tag if this file is first in the list */
            *out << s.substr(0, pos_start);

            /* all elements are set to hidden before any element loads
                or otherwise Chrome renders elements on the fly as SVG loads */
            *out << "<defs><style type=\"text/css\">";
            int len = argc - optind;
            for (int j = 0; j < len; j++) {
                *out << "#" << idprefix << j << ",";
            }
            *out << "_{visibility:hidden}</style></defs>";
        }

        /* find the first occurrence of `</svg>` in string */
        size_t pos_end = s.find("</svg>");

        /* unwrap `<svg>` tag in string */
        s = s.substr(pos_start, pos_end - pos_start);

        /* add prefix to all element IDs to avoid conflict among frames 
            and update all links to an element with prefixed element IDs */
        const std::string attrs[] = {
            " id=\"", 
            " href=\"#", 
            " xlink:href=\"#", 
        };
        for (size_t j = 0; j < sizeof(attrs) / sizeof(attrs[0]); j++) {
            string_replace(s, attrs[j], attrs[j] + idprefix + std::to_string(idx));
        }

        /* output frame wrapped in a `<g>` tag for grouping */
        *out << "<g id=\"" << idprefix << idx << "\">" << s << "</g>";
    }

    /* output CSS animation definitions with no unnecessary whitespace
        note that animation is defined after other elements because otherwise 
        heavy flickering seen in Chrome due to animation start time mismatch */
    *out << "<defs><style type=\"text/css\">";
    int len = argc - optind;
    for (int j = 0; j < len; j++) {
        *out << "#" << idprefix << j << ",";
    }
    *out << "_{animation:" << (delayseconds * len) << "s linear k infinite}";
    for (int j = 0; j < len; j++) {
        *out << "#" << idprefix << j << "{";
        *out << "animation-delay:" << (delayseconds * j) << "s}";
    }
    *out << "@keyframes k{" << std::fixed;
    *out << "0%," << (100.0 / len) << "%{visibility:visible}";
    *out << ((100.0 + TRANSITION_PERCENT) / len) << "%,100%{visibility:hidden}";
    *out << "}</style></defs>";

    *out << "</svg>";
}
