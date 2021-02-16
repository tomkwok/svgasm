#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

#define BUFFER_LEN 1024
#define TRANSITION_PERCENT 0.0001

#define DELAY_SECS "0.5"
#define STDOUT_NAME "-"
#define ID_PREFIX "_"
#define ITER_COUNT "infinite"
#define LOADING_TEXT "Loading ..."
#define CLEANER_CMD "svgcleaner --multipass -c %s"
#define TRACER_CMD "cat %s"

#define HELP_CONTENT "svgasm [options] infilepath...\n\n" \
    "Options:\n" \
    "  -d <delaysecs>     animation delay in seconds  (default: " DELAY_SECS ")\n" \
    "  -o <outfilepath>   path to SVG animation output file " \
                            "or " STDOUT_NAME " for stdout  (default: " STDOUT_NAME ")\n" \
    "  -p <idprefix>      prefix added to element IDs  (default: " ID_PREFIX ")\n" \
    "  -i <itercount>     animation iteration count  (default: " ITER_COUNT ")\n" \
    "  -l <loadingtext>   loading text in output  (default: '" LOADING_TEXT "')\n" \
    "  -c <cleanercmd>    command for cleaner with '%s'  (default: '" CLEANER_CMD "')\n" \
    "  -t <tracercmd>     command for tracer with '%s'  (default: '" TRACER_CMD "')\n" \
    "  -h                 print help information\n"

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

inline double parse_fraction (std::string s) {
    size_t pos = s.find("/");
    if (pos == std::string::npos) {
        return atof(s.c_str());
    } else {
        double a = atof(s.substr(0, pos).c_str());
        double b = atof(s.substr(pos+1).c_str());
        return a / b;
    }
}

int main (int argc, char *argv[]) {
    double delaysecs = atof(DELAY_SECS);
    char outfilepath[BUFFER_LEN] = STDOUT_NAME;
    char idprefix[BUFFER_LEN] = ID_PREFIX;
    char itercount[BUFFER_LEN] = ITER_COUNT;
    char loadingtext[BUFFER_LEN] = LOADING_TEXT;
    char cleanercmd[BUFFER_LEN] = CLEANER_CMD;
    char tracercmd[BUFFER_LEN] = TRACER_CMD;
    int c;
    while ((c = getopt(argc, argv, "d:o:p:i:l:c:t:h")) != -1) {
        switch (c) {
            case 'd':
                double d;
                d = parse_fraction(optarg);
                if (d > 0.0) {
                    delaysecs = d;
                } else {
                    std::cerr << "Argument -d parsing failed." << std::endl;
                    exit(0);
                }
                break;
            case 'o':
                std::strncpy(outfilepath, optarg, BUFFER_LEN);
                break;
            case 'p':
                std::strncpy(idprefix, optarg, BUFFER_LEN);
                break;
            case 'i':
                std::strncpy(itercount, optarg, BUFFER_LEN);
                break;
            case 'l':
                std::strncpy(loadingtext, optarg, BUFFER_LEN);
                break;
            case 'c':
                std::strncpy(cleanercmd, optarg, BUFFER_LEN);
                break;
            case 't':
                std::strncpy(tracercmd, optarg, BUFFER_LEN);
                break;
            case 'h':
            default:
                std::cout << HELP_CONTENT;
                exit(0);
                break;
        }
    }

    if (argc - optind < 2) {
        std::cout << HELP_CONTENT;
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

            /* output loading text, which would be set to hidden at the end of file */
            if (loadingtext[0] != '\0') {
                *out << "<text x=\"50%\" y=\"50%\" style=\"font-family:sans-serif\" ";
                *out << "dominant-baseline=\"middle\" text-anchor=\"middle\" ";
                *out << "id=\"" << idprefix << "\">" << loadingtext << "</text>";
            }

            /* all elements are set to hidden before any element loads
                or otherwise Chrome starts timing animation of elements as SVG loads */
            *out << "<defs><style type=\"text/css\">";
            int len = argc - optind;
            for (int j = 0; j < len; j++) {
                if (j > 0) {
                    *out << ",";
                }
                *out << "#" << idprefix << j;
            }
            *out << "{visibility:hidden}</style></defs>";
        }

        /* find the first occurrence of `</svg>` in string */
        size_t pos_end = s.find("</svg>");

        /* unwrap `<svg>` tag in string */
        s = s.substr(pos_start, pos_end - pos_start);

        /* add prefix to element IDs to avoid conflict since IDs are global in an SVG
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
        if (j > 0) {
            *out << ",";
        }
        *out << "#" << idprefix << j;
    }
    *out << "{animation:" << (delaysecs * len) << "s linear k " << itercount << "}";
    for (int j = 0; j < len; j++) {
        *out << "#" << idprefix << j << "{";
        *out << "animation-delay:" << (delaysecs * j) << "s}";
    }
    *out << "@keyframes k{" << std::fixed;
    *out << "0%," << (100.0 / len) << "%{visibility:visible}";
    *out << ((100.0 + TRANSITION_PERCENT / delaysecs) / len) << "%,100%";
    *out << "{visibility:hidden}}";
    if (loadingtext[0] != '\0') {
        *out << "#" << idprefix << "{visibility:hidden}";
    }
    *out << "</style></defs>";

    *out << "</svg>";

    return 0;
}
