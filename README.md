# *svgasm* &nbsp; [![Build status of this repository](https://github.com/tomkwok/svgasm/workflows/svgasm/badge.svg)](https://github.com/tomkwok/svgasm/actions?query=branch%3Amaster)

SVG animation from multiple still SVGs or single GIF using tracer

[***svgasm***](https://github.com/tomkwok/svgasm) is a proof-of-concept SVG assembler to generate a self-contained animated SVG file from multiple still SVG files with CSS keyframes animation to play frames in sequence.

- Produces single animated SVG file that is viewable in Chrome, Safari, Firefox, Edge and IE 10+.
- Executes SVG cleaner [*svgcleaner*](https://github.com/RazrFalcon/svgcleaner) (by default) or [*svgo*](https://github.com/svg/svgo) to minify each SVG file.
- Executes bitmap tracer [*potrace*](http://potrace.sourceforge.net/) (by default) or [*autotrace*](https://github.com/autotrace/autotrace), or [*primitive*](https://github.com/fogleman/primitive) to convert raster image input to SVG.

## Usage

```
svgasm [options] infilepath...

Options:
  -d <delaysecs>     animation delay in seconds  (default: 0.5)
  -o <outfilepath>   path to SVG animation output file or - for stdout  (default: -)
  -p <idprefix>      prefix added to element IDs  (default: _)
  -i <itercount>     animation iteration count  (default: infinite)
  -e <endframe>      index of frame to stop at in last iteration if not infinite  (default: -1)
  -l <loadingtext>   loading text in output or '' to turn off  (default: 'Loading ...')
  -c <cleanercmd>    command for SVG cleaner with "%s"  (default: 'svgcleaner --multipass -c "%s"')
  -t <tracercmd>     command for tracer for non-SVG file with "%s"  (default: '')
  -h                 print help information
```

## Usage examples

- `svgasm -d 2 -i 5 -e 0 -o animation.svg input1.svg input2.svg input3.svg`  
Generates output *animation.svg* from *input1.svg*, *input2.svg* and *input3.svg* that animates with a delay of 2 seconds per frame, iterates 5 times, and stops at the first frame in the last iteration. 
- `svgasm -d 1/30 -l '' frame*.png > animation.svg`  
Generates output *animation.svg* from wild card *frame\*.png* (in alphabetical order) that animates with 30 frames per second, iterates infinitely, and with loading text turned off. The default tracer command for *potrace* is used.
- `svgasm animation.gif > animation.svg`  
Generates output *animation.svg* from *animation.gif* that animates with the same speed and iteration count.

## Output examples

### SVG animation from multiple still SVGs

An example of a 2-fps 2-frame animated build status badge for this project is generated using ***svgasm*** from 2 [GitHub Workflows status badge](https://docs.github.com/en/actions/managing-workflow-runs/adding-a-workflow-status-badge) SVG files.

![Build status badge animation example](examples/badge_animation.svg)

A worked example of a 1-fps 2-frame animated calendar plot in [examples/calplot_animation.ipynb](examples/calplot_animation.ipynb) is generated using [calplot](https://github.com/tomkwok/calplot) and ***svgasm***.

![Calplot animation example](examples/calplot_animation.svg)

While the above two examples can be created with CSS animation with some effort without the approach adopted in ***svgasm***, the following example is not. (An alternative approach would be to completely re-program plot generation with JavaScript such as with a [D3.js library](https://github.com/d3/d3-contour) to have the browser perform calculations to generate the values of text, paths and gradients on the fly.)

The following example is a 30-fps 41-frame animated contour plot of [F<sub>&beta;</sub> score](https://en.wikipedia.org/wiki/F-score) pre-generated using some custom contour plot function that is also built on [matplotlib](https://github.com/matplotlib/matplotlib) and animated in sequence with ***svgasm***.

![Contour plot animation example](examples/contour_f_beta_animation.svg)


## Installing on macOS

To install and run a pre-compiled binary of ***svgasm*** on macOS with [Homebrew](https://brew.sh/) installed, run the following commands, which would also install the default runtime dependencies of ***svgasm***:

```sh
brew install tomkwok/tap/svgasm
svgasm
```

## Building

To build ***svgasm*** with a C++98 complier, change `clang++` in `Makefile` to your installed compiler, and run the following commands:

```sh
git clone https://github.com/tomkwok/svgasm
cd svgasm/
make svgasm
./svgasm
```

Before running ***svgasm***, install runtime dependencies. An example instruction is provided for [Arch Linux](https://archlinux.org/) as follows:

```sh
sudo pacman -S svgcleaner potrace imagemagick
```

## Installing optional runtime dependencies

- [*svgo*](https://github.com/svg/svgo): running the command `npm install -g svgo` or `yarn global add svgo`
- [*autotrace*](https://github.com/autotrace/autotrace): downloading a release binary from the [release page of autotrace](https://github.com/autotrace/autotrace/releases) to one of the directories in `PATH` 
- [*primitive*](https://github.com/fogleman/primitive): running the command `go get -u github.com/fogleman/primitive`

## Benchmark with different SVG cleaners

***svgasm*** has been tested to work with the following cleaners:

- ***svgcleaner*** is the default cleaner program. *svgcleaner* is chosen as the default cleaner because it is generally very fast and produces small output.
- ***svgo*** can be specified as the cleaner program. *svgo* is typically over an order of magnitude slower than *svgcleaner*. Nonetheless, *svgo* can sometimes produce smaller files than *svgcleaner* does.
- ***cat*** can be specified as a dummy cleaner program to generate SVG animation from SVG files that are already minified. (In particular, in each input SVG file, there are no tags like `<?xml`... or comments before `<svg`..., and no unnecessary white-spaces as in `id = "`... or `</ svg>`.)

The following are the results of using ***svgasm*** to produce the 41-frame contour animation example above on an Intel Core i5 processor with different cleaners:

- `svgasm -c 'svgcleaner --multipass -c %s' ...`: output file size 2,232 KiB, executed in 0.844 s ± 0.041 s.
- `svgasm -c 'svgo --multipass -o - %s' ...`: output file size 2,054 KiB, executed in 34.839 s ± 0.292 s.
- `svgasm -c 'cat %s' ...`: process already minified input files, executed in 0.265 s ± 0.006 s.


## External links

- An APNG assembler [*apngasm*](https://github.com/apngasm/apngasm). The name of ***svgasm*** is inspired by *apngasm*. It is a sexy name with no pun intended. It can also mean ***svg*** is ***a***we***s***o***m***e as a more platonic interpretation.
- A command-line benchmarking tool [hyperfine](https://github.com/sharkdp/hyperfine) that runs commands at least 10 times and presents time measurements in the format of mean ± sigma (1 standard deviation).
- My proposal of [Tom's rainbow color map](https://tomkwok.com/posts/color-maps/), which is used in the contour plot animation example above.
- An [example](https://stackoverflow.com/questions/48893587/simple-animate-multiple-svgs-in-sequence-like-a-looping-gif) on Stack Overflow of animating a sequence of hand-drawn vector graphics. The principal method employed in the ***svgasm*** tool of playing SVGs in sequence with CSS animation is credited to its author.

## How ***svgasm*** is implemented

- Implementation is fine-tuned in the ***svgasm*** tool to better accommodate browser behavior. See inline comments in [src/svgasm.cpp](src/svgasm.cpp) for details. In particular, since Chrome renders elements on the fly and starts timing animations on elements as SVG file loads, attention was given in styles placement to prevent flickering in Chrome due to mismatching animation start times for different frames. Note that such flickering is only apparent in an animation with a high fps value. (It is not clear in the [CSS Animations specification](https://www.w3.org/TR/css-animations-1/#animations) whether a style rule is considered resolved or not when the styles are parsed but the elements referenced are not yet in the DOM. The answer is apparently no in Chrome, so CSS animations need to be moved to the end for animation of frames to start at the same time.)
- An SVG file cleaner is executed to pre-process SVG files so that the main algorithm of proof-of-concept tool ***svgasm*** can be implemented using mainly string operations in standard library instead of an XML parser. The animated SVG file output generated using this method does not seem to be further minifiable using *svgcleaner* or *svgo*. Currently, *svgo* gives an empty SVG when given the output of ***svgasm***.
- The `<svg>` tag including its attributes (such as `width`, `height` and `viewbox`) in the first SVG file in the sequence of command-line arguments to the ***svgasm*** tool is copied to the output. It is assumed that all frames have the same size and viewport bounds, or all frames are effectively cropped to the size of the first frame. This approach is taken for simplicity in this proof-of-concept implementation and can be improved.
- The ***svgasm*** tool generates a self-contained output file with a configurable loading text, the content of all SVG frames and CSS animation styles in `<style>`. It does not add any `<object>` or `<script>` tags.

## End-of-animation frame index configurable in SVG output

- The end-of-animation index feature with a default value of -1 in the ***svgasm*** tool is necessary to recreate the behavior in GIF that animation stops at the last frame rather than disappears after loops are completed.
- This feature is extended to accept a configurable end-of-animation frame index *e* specified in argument `-e`, for which -*x* denotes the *x*-th last frame and *0* denotes the first frame. This feature can be turned off with argument `-e ''`  when executing *svgasm* so that the output animation becomes invisible after a predefined number of iterations are completed. This feature has no effect on an animation that loops infinitely.

## Note on CSS animation approach adopted

- While the CSS animation rules to implement the end-of-frame index feature may look simple and obvious in source code, it is tricky to derive them. This is likely due to the fact that CSS rules are imperative. An explicit expression of the mathematical formula for animation delay and iteration count for individual frames are required for CSS animation, which does not natively support chained animations. In contrast, the description of animation in human language as in the help information of this tool is procedural. A procedural implementation of animation in JavaScript, which is avoided in this tool, would probably be more straight forward.
- CSS animation as defined in the W3C specification (or similarly in [SMIL](https://www.w3.org/TR/SMIL3/)) is designed for interpolated transition of computed property values rather than for GIF-style discretized animation of a sequence of still images adopted in the ***svgasm*** tool. Nonetheless, in order to increase usable fps of the resulting animation, an approach to be explored is using deep generative networks like [DeepSVG](https://github.com/alexandre01/deepsvg) (2020) to generate interpolated SVG frames to be inserted in between existing SVG frames from input. (Interestingly, all the animated SVG interpolation demos on the [DeepSVG project webpage](https://web.archive.org/web/20201118113845/https://blog.alexandrecarlier.com/deepsvg/) are presented in GIF format. The ***svgasm*** tool would fit in this case as it can be used to create an animated SVG from multiple still SVGs.)

## Loading text configurable in SVG output

- By default, the output of ***svgasm*** contains a configurable loading text ("Loading ..." by default) displayed in sans serif font, which is especially useful for a large SVG file since downloading and parsing take some time in a browser. Notably, this is implemented by careful placement of CSS styles and with no JavaScript.
- The text element and style cost a total of 147 bytes with default `loadingtext` and `idprefix`. The loading text in SVG output can be turned off with argument `-l ''` when executing *svgasm*.
- The loading text can be seen when the file is loading in Chrome, Firefox or Edge. It cannot be seen in Safari or IE 10+, which waits until SVG is completely loaded before displaying any elements.
- The loading text cannot be viewed on GitHub since it is replaced with GitHub's own spinning wheel image when an SVG file is loading. Instead, see custom loading text in action in external file [contour_itercount_2.svg](https://d33wubrfki0l68.cloudfront.net/a5708d35e08996ebb3e7eb4d26194e97c55ef56e/669a6/plot/iou-vs-f1/contour_itercount_2.svg). Also, a preview of the default loading text is provided as follows.

![Loading text preview](readme/loading_text.svg)

## Origin of the ***svgasm*** tool

- The ***svgasm*** tool was originally created to automate the generation of an SVG animation composed of a relatively high number of SVG files, that is the shown contour plot animation example. (SVG animation is [not officially supported in *matplotlib*](https://github.com/matplotlib/matplotlib/pull/4255).) The original program was created in one day, and the flickering issue in Chrome was fixed in the next day. The program at that point can be seen in the first commit of this repository.
- It was found that such animation of moderately complicated SVG content did not perform as horribly as expected in browsers, and it works even when played with a relatively high fps on a computer with no dedicated GPU. Therefore, this project was created to present the ***svgasm*** tool to easily generate discretized SVG animation from stills as a viable alternative to raster animation formats like GIF, APNG or video formats. There is potential for more use cases.

## Note on programming language choice

- The implementation of the ***svgasm*** tool started with C++ instead of an interpreted scripting language. This was to avoid having the simple operations in ***svgasm*** take a longer time to run than the running time of the much more technically complex SVG parsing and cleaning tool *svgcleaner* for typical SVG files.
- To maximize compatibility with compilers on older systems, features in C++11 or newer are deliberately avoided. For example, `std::stringstream` is used instead of `std::to_string` in C++11, and explicit assignment for map values is used instead of initializer lists in C++11. This is to ensure that C++98 support is sufficient to compile the ***svgasm*** tool.

## To-dos

Reduce output file size
- Use all alphanumeric characters in prefix for element IDs.
- De-duplication of repeated elements across frames (needs parser).

Reduce output generation time
- Parallelize cleaner and tracer execution for input files on multiple threads.

Improve terminal I/O
- Support building and running on Windows.
- Command line argument input sanitization.
- Argument option `-q` for quietness to suppress standard error output.
- Progress bar for processing input files.

Add more functionalities
- A GUI front-end for the *svgasm* tool.
- Support configurable output size and viewport bounds.
- Support frame extraction from animated PNG and video files.
- Assembler to combine two or more SVG animations in sequence.
- Disassembler for SVG animation created with *svgasm*.
- Support automatic insertion of interpolated frames to increase usable fps.
