/* Copyright (C) 2021 tom [at] tomkwok.com */

#include <cstdio>
#include <cstring>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

/* POSIX headers */
#include <getopt.h>
#include <dirent.h>

#define BUFFER_LEN 1024
#define TRANSITION_PERCENT 0.0001
#define SVG_SUFFIX ".svg"
#define CLEANER_CMD_FALLBACK "cat \"%s\""

#define DELAY_SECS "0.1"
#define STDIO_NAME "-"
#define ID_PREFIX "_"
#define ITER_COUNT "infinite"
#define END_FRAME "-1"
#define LOADING_TEXT "Loading ..."
#define CLEANER_CMD "svgcleaner --multipass -c \"%s\""
#define TRACER_CMD "gm convert +matte \"%s\" pgm:- | " \
    "mkbitmap -s 1 - -o - | potrace --svg -o -"
#define MAGICK_CMD "gm %s"

#define GIF_SUFFIX ".gif"
#define TEMP_DIR "/tmp/svgasm-XXXXXX"
#define FRAME_FILENAME "%s/%d.png"
#define MAGICK_CMD_CONVERT "convert \"%s\" -coalesce +adjoin \"%s/%%d.png\""
#define MAGICK_CMD_IDENTIFY "identify -format \"%%T\\n\" \"%s\""

#define HELP_CONTENT "svgasm [options] infilepath...\n\n" \
    "Options:\n" \
    "  -d <delaysecs>     animation time delay in seconds  (default: " DELAY_SECS ")\n" \
    "  -o <outfilepath>   path to SVG animation output file " \
                            "or " STDIO_NAME " for stdout  (default: " STDIO_NAME ")\n" \
    "  -p <idprefix>      prefix added to element IDs  (default: " ID_PREFIX ")\n" \
    "  -i <itercount>     animation iteration count  (default: " ITER_COUNT ")\n" \
    "  -e <endframe>      index of frame to stop at in last iteration if not infinite" \
                            "  (default: " END_FRAME ")\n" \
    "  -l <loadingtext>   loading text in output or '' to turn off" \
                            "  (default: '" LOADING_TEXT "')\n" \
    "  -c <cleanercmd>    command for SVG cleaner with \"%s\"" \
                            "  (default: '" CLEANER_CMD "')\n" \
    "  -t <tracercmd>     command for tracer for non-SVG still image with \"%s\"" \
                            "  (default: '" TRACER_CMD "')\n" \
    "  -m <magickcmd>     command for magick program for GIF animation with %s" \
                            "  (default: '" MAGICK_CMD "')\n" \
    "  -h                 print help information\n"

inline std::string exec (const char* cmd, bool exit_on_fail) {
    std::cerr << cmd << std::endl;
    #ifdef _WIN32
        #define POPEN _popen
        #define PCLOSE _pclose
    #else
        #define POPEN popen
        #define PCLOSE pclose
    #endif
    char buffer[BUFFER_LEN];
    std::string result = "";
    FILE* pipe = POPEN(cmd, "r");
    if (!pipe) {
        std::cerr << "popen() failed.";
    }
    try {
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        PCLOSE(pipe);
        if (exit_on_fail) {
            std::cerr << "Command execution failed." << std::endl;
            exit(0);
        } else {
            return "";
        }
    }
    PCLOSE(pipe);
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

inline bool string_endswith_lowercase (std::string& s, const std::string& suffix) {
    size_t a = s.length();
    size_t b = suffix.length();
    if (a < b) {
        return false;
    } else {
        std::string sub = s.substr(a - b);
        /* a file extension is case insensitive; assume lower case suffix */
        std::transform(sub.begin(), sub.end(), sub.begin(), tolower);
        return (sub == suffix);
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

inline void assert_not_string_npos (size_t& pos) {
    if (pos == std::string::npos) {
        std::cerr << "Input file parsing failed." << std::endl;
        exit(0);
    }
}

int main (int argc, char *argv[]) {
    bool
        auto_delaysecs = true,
        auto_cleanercmd = true;

    double delaysecs = atof(DELAY_SECS);

    std::string
        outfilepath = STDIO_NAME,
        idprefix = ID_PREFIX,
        itercount = ITER_COUNT,
        endframe = END_FRAME,
        loadingtext = LOADING_TEXT,
        cleanercmd = CLEANER_CMD,
        tracercmd = TRACER_CMD,
        magickcmd = MAGICK_CMD;

    std::map<char, std::string*> m;
    m['o'] = &outfilepath;
    m['p'] = &idprefix;
    m['i'] = &itercount;
    m['e'] = &endframe;
    m['l'] = &loadingtext;
    m['c'] = &cleanercmd;
    m['t'] = &tracercmd;
    m['m'] = &magickcmd;

    std::string optstring = "hd:";
    for (std::map<char, std::string*>::iterator it = m.begin(); it != m.end(); ++it) {
        optstring += it->first;
        optstring += ":";
    }

    int c;
    while ((c = getopt(argc, argv, optstring.c_str())) != -1) {
        if (c == 'h') {
            std::cout << HELP_CONTENT;
            exit(0);
        } else if (c == 'd') {
            double d;
            d = parse_fraction(optarg);
            if (d > 0.0) {
                delaysecs = d;
                auto_delaysecs = false;
            } else {
                std::cerr << "Argument -d parsing failed." << std::endl;
                exit(0);
            }
        } else {
            *m[c] = optarg;
            if (c == 'c') {
                auto_cleanercmd = false;
            }
        }
    }

    /* ensure at least one file in arguments */
    if (argc - optind < 1) {
        std::cout << HELP_CONTENT;
        exit(0);
    }

    std::ostream* out;
    std::ofstream outfile;
    if (outfilepath == STDIO_NAME) {
        out = &std::cout;
    } else {
        outfile.open(outfilepath.c_str());
        if (!outfile.is_open()) {
            std::cerr << "Output file creation failed." << std::endl;
            exit(0);
        }
        out = &outfile;
    }

    std::vector<std::string> filepaths;
    for (int i = optind; i < argc; i++) {
        std::string filepath = argv[i];

        /* run GraphicsMagick or ImageMagick on a GIF file to convert it to frames */
        if (string_endswith_lowercase(filepath, GIF_SUFFIX)) {
            char cmd_args[BUFFER_LEN];
            char cmd[BUFFER_LEN];

            if (auto_delaysecs) {
                /* get delay from the first GIF file in args rather than the last */
                auto_delaysecs = false;
                int count = std::snprintf(cmd_args, BUFFER_LEN, MAGICK_CMD_IDENTIFY,
                    filepath.c_str());
                std::snprintf(cmd, BUFFER_LEN-count, magickcmd.c_str(), cmd_args);
                std::string s = exec(cmd, true);
                size_t pos = s.find("\n");
                assert_not_string_npos(pos);
                int d = atoi(s.substr(0, pos).c_str());
                if (d > 0) {
                    delaysecs = d * 0.01;
                }
            }

            char tempdir[] = TEMP_DIR;
            if (mkdtemp(tempdir) == NULL) {
                std::cerr << "Temporary directory creation failed." << std::endl;
                exit(0);
            }

            int count = std::snprintf(cmd_args, BUFFER_LEN, MAGICK_CMD_CONVERT,
                filepath.c_str(), tempdir);
            std::snprintf(cmd, BUFFER_LEN-count, magickcmd.c_str(), cmd_args);
            exec(cmd, true);

            /* count number of files in tempdir, which should be the number of frames */
            int counter = 0;
            DIR *dir;
            dir = opendir(tempdir);
            if (!dir) {
                std::cerr << "Temporary directory access failed." << std::endl;
                exit(0);
            }
            struct dirent *d;
            while ((d = readdir(dir)) != NULL) {
                if (d->d_name[0] != '.') {
                    counter++;
                }
            }
            closedir(dir);

            /* generate filenames in order of index */
            for (int j = 0; j < counter; j++) {
                char filename[BUFFER_LEN];
                std::snprintf(filename, BUFFER_LEN, FRAME_FILENAME, tempdir, j);
                filepaths.push_back(filename);
            }
        } else {
            filepaths.push_back(filepath);
        }
    }

    std::string closing_tags;
    int len = filepaths.size();
    for (int i = 0; i < len; i++) {
        char cmd[BUFFER_LEN];
        std::string& filepath = filepaths[i];

        /* run SVG cleaner on input file to obtain a clean optimized file
            without unnecessary white spaces in tags */
        if (string_endswith_lowercase(filepath, SVG_SUFFIX)) {
            std::snprintf(cmd, BUFFER_LEN, cleanercmd.c_str(), filepath.c_str());
        } else {
            size_t pos = 0;
            pos += std::snprintf(&cmd[pos], BUFFER_LEN-pos,
                tracercmd.c_str(), filepath.c_str());
            pos += std::snprintf(&cmd[pos], BUFFER_LEN-pos, " | ");
            pos += std::snprintf(&cmd[pos], BUFFER_LEN-pos,
                cleanercmd.c_str(), STDIO_NAME);
        }

        std::string s;
        if (auto_cleanercmd) {
            /* check for cleaner fallback is done only once */
            auto_cleanercmd = false;
            s = exec(cmd, false);
            if (s == "") {
                /* change cleaner to fallback and try again */
                cleanercmd = CLEANER_CMD_FALLBACK;
                i--;
                continue;
            }
        } else {
            s = exec(cmd, true);
        }

        /* find the first occurrence of `<svg` */
        size_t pos_tag = s.find("<svg");
        assert_not_string_npos(pos_tag);

        /* find the first occurrence of `>` after `<svg` */
        size_t pos_start = s.find(">", pos_tag) + 1;
        assert_not_string_npos(pos_start);

        if (i == 0) {
            /* output opening `<svg>` tag if this file is first in the list */
            *out << s.substr(0, pos_start);

            /* output loading text, which would be set to hidden at the end of file */
            if (loadingtext[0] != '\0') {
                *out << "<text x=\"50%\" y=\"50%\" style=\"font-family:sans-serif\" ";
                *out << "dominant-baseline=\"middle\" text-anchor=\"middle\" ";
                *out << "id=\"" << idprefix << "\">" << loadingtext << "</text>";
            }

            /* all elements are set to hidden before any element for a frame loads
                or otherwise Chrome starts timing animation of elements as SVG loads */
            *out << "<defs><style type=\"text/css\">";
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
        assert_not_string_npos(pos_end);

        if (i == 0) {
            /* save ending tags (`</svg>`, and others if any) for output at the end */
            closing_tags = s.substr(pos_end);
        }

        /* unwrap `<svg>` tag in string and mutate string */
        s = s.substr(pos_start, pos_end - pos_start);

        /* add prefix to element IDs to avoid conflict since IDs are global in an SVG
            and update all links to an element with prefixed element IDs */
        const std::string attrs[] = {
            " id=\"", 
            " href=\"#", 
            " xlink:href=\"#", 
            "=\"url(#", /* example attributes: fill, clip-path */
        };
        for (size_t j = 0; j < sizeof(attrs) / sizeof(attrs[0]); j++) {
            std::ostringstream ss;
            ss << attrs[j] << idprefix << i;
            string_replace(s, attrs[j], ss.str());
        }

        /* output frame wrapped in a `<g>` tag for grouping */
        *out << "<g id=\"" << idprefix << i << "\">" << s << "</g>";
    }

    /* output CSS animation definitions with no unnecessary whitespace
        note that animation is defined after frame groups because otherwise
        heavy flickering seen in Chrome due to animation start time mismatch */
    *out << "<defs><style type=\"text/css\">";

    int e = len;
    /* end frame index can be defined for non-infinite animation */
    if (endframe[0] != '\0' && itercount != "infinite") {
        /* add length to negative end frame index and wrap out-of-bound index */
        e = (atoi(endframe.c_str()) + len) % len;
    }

    for (int j = 0; j < len; j++) {
        if (j > 0) {
            *out << ",";
        }
        *out << "#" << idprefix << j;
    }
    *out << "{animation:";
    *out << (delaysecs * len) << "s linear " << idprefix << "k " << itercount << "}";

    int ic;
    if (e < len) {
        ic = atoi(itercount.c_str());
        if (ic < 1) {
            ic = 1;
        }
        /* each of the frames after end frame has its itercount decremented by 1 */ 
        for (int j = e + 1; j < len; j++) {
            if (j > e + 1) {
                *out << ",";
            }
            *out << "#" << idprefix << j;
        }
        if (e + 1 < len) {
            *out << "{animation-iteration-count:" << (ic - 1) << "}";
        }

        /* animation for end frame has two parts (loop and once forward) */
        *out << "#" << idprefix << e;
        *out << "{animation:";
        *out << (delaysecs * len) << "s linear " << idprefix << "k " << (ic - 1) << ",";
        *out << "0s linear forwards " << idprefix << "e}";
        *out << "@keyframes " << idprefix << "e{to{visibility:visible}}";
    }

    for (int j = 0; j < len; j++) {
        *out << "#" << idprefix << j << "{";
        *out << "animation-delay:" << (delaysecs * j) << "s";
        if (j == e) {
            /* add delay component for second animation for end frame */
            *out << "," << (delaysecs * (len * (ic - 1) + e)) << "s";
        }
        *out << "}";
    }
    *out << "@keyframes " << idprefix << "k{" << std::fixed;
    *out << "0%," << (100.0 / len) << "%{visibility:visible}";
    *out << ((100.0 + TRANSITION_PERCENT / delaysecs) / len) << "%,100%";
    *out << "{visibility:hidden}}";

    /* hide loading text since all elements have loaded before this style is parsed */
    if (loadingtext[0] != '\0') {
        *out << "#" << idprefix << "{visibility:hidden}";
    }

    *out << "</style></defs>";
    *out << closing_tags;

    std::cerr << "SVG animation output saved to " << outfilepath;
    std::cerr << std::fixed << std::setprecision(2);
    if (outfilepath != STDIO_NAME) {
        std::cerr << " (" << (out->tellp() / 1024.0) << " KiB) " << std::endl;
    }

    return 0;
}
