# svgasm

SVG animation from multiple SVGs or single GIF using tracer

[***svgasm***](https://github.com/tomkwok/svgasm) is a proof-of-concept SVG assembler to generate a self-contained animated SVG file from multiple SVG files with CSS keyframes animation to play frames in sequence.

- Produces single animated SVG file that is viewable in Chrome, Safari, Firefox and IE 10+.
- Executes SVG file cleaner [*svgcleaner*](https://github.com/RazrFalcon/svgcleaner) (by default) or [*svgo*](https://github.com/svg/svgo) to minify each SVG file.
- Executes bitmap tracer [*potrace*](http://potrace.sourceforge.net/) (by default) or [*autotrace*](https://github.com/autotrace/autotrace) to convert raster image input to SVG.

## Usage

```
svgasm [options] infilepath...

Options:
  -d <delaysecs>     animation delay in seconds  (default: 0.5)
  -o <outfilepath>   path to SVG animation output file or - for stdout  (default: -)
  -p <idprefix>      prefix added to element IDs  (default: _)
  -i <itercount>     animation iteration count  (default: infinite)
  -l <loadingtext>   loading text in output  (default: 'Loading ...')
  -c <cleanercmd>    command for cleaner with '%s'  (default: 'svgcleaner --multipass -c %s')
  -t <tracercmd>     command for tracer with '%s'  (default: 'cat %s')
  -h                 print help information
```

## Usage examples

```sh
svgasm -d 2 -o animation.svg input1.svg input2.svg input3.svg
svgasm -d 1/30 frame*.svg > animation.svg
svgasm animation.gif > animation.svg
```

## Output examples

### SVG animation from multiple SVGs

The following command is executed in a [Jupyter Notebook](https://jupyter.org/) to use ***svgasm*** to produce a 30-fps SVG animation from 41 pre-generated contour plot SVG files, executed via `time` command for benchmarking.

```python
!time svgasm -o animation.svg -d 1/30 {' '.join([f'animation/F{beta}.svg' for beta in betas])}
```

![Contour plot animation example](examples/contour_f_beta_animation.svg)

## Install on macOS

To install and run a pre-compiled binary of ***svgasm*** on macOS with [Homebrew](https://brew.sh/) installed, run the following commands, which would also install the default runtime dependencies of ***svgasm***:

```sh
brew install tomkwok/tap/svgasm
svgasm
```

## Building

To build ***svgasm*** with a C++11 complier, change `clang++` in `Makefile` to your installed compiler, and run the following commands:

```sh
git clone https://github.com/tomkwok/svgasm
cd svgasm/
make svgasm
```

Before running ***svgasm***, install runtime dependencies. An example instruction is provided for [Arch Linux](https://archlinux.org/) as follows:

```sh
sudo pacman -S svgcleaner potrace imagemagick
```

## Benchmark with different cleaners

***svgasm*** has been tested to work with the following cleaners:

- ***svgcleaner*** is the default cleaner program. *svgcleaner* is chosen as the default cleaner because it is generally very fast and produces small output.
- ***svgo*** can be specified as the cleaner program. *svgo* is typically over an order of magnitude slower than *svgcleaner*. Nonetheless, *svgo* can sometimes produce smaller files than *svgcleaner* does.
- ***cat*** can be specified as a dummy cleaner program to generate SVG animation from SVG files that are already minified. (In particular, in each input SVG file, there are no tags like `<?xml`... or comments before `<svg`..., and no unnecessary white-spaces as in `id = "`... or `</ svg>`.)

The following are the results of using ***svgasm*** to produce the 41-frame contour animation example above on an Intel Core i5 processor with different cleaners:

- `svgasm -c 'svgcleaner --multipass -c %s' ...`: output with file size 2232 KiB, executed in 1.20 seconds.
- `svgasm -c 'svgo --multipass -o - %s' ...`: output with file size 2054 KiB, executed in 39.04 seconds.
- `svgasm -c 'cat %s' ...`: process already minified input files, executed in 0.43 seconds.


## External links

- The APNG assembler [*apngasm*](https://github.com/apngasm/apngasm). The name of ***svgasm*** is inspired by *apngasm*. It is a sexy name.
- An unmerged [pull request to *matplotlib* for SVG animation output](https://github.com/matplotlib/matplotlib/pull/4255). The lack of support for SVG animation in matplotlib was the motivation for the creation of the ***svgasm*** tool.
- An [example](https://stackoverflow.com/questions/48893587/simple-animate-multiple-svgs-in-sequence-like-a-looping-gif) on Stack Overflow of animating a sequence of hand-drawn vector graphics.
- The proposal of [Tom's rainbow color map](https://tomkwok.com/posts/color-maps/), which is used in the contour plot animation example above.

## Implementation notes

- The method of playing SVGs in sequence with CSS animation employed in the ***svgasm*** tool is credited to the author of the linked Stack Overflow example.
- Implementation is fine-tuned in the ***svgasm*** tool to better accommodate browser behavior. See inline comments in `svgasm.cpp` for details. In particular, since Chrome renders elements on the fly and starts timing animations on elements as SVG file loads, attention was given in styles placement to prevent heavy flickering in Chrome due to mismatching animation start times for different frames. (It is not clear in the [CSS Animations specification](https://www.w3.org/TR/css-animations-1/#animations) whether a style rule is considered resolved or not when the styles are parsed but the elements referenced are not yet in the DOM. The answer is apparently no in Chrome, so CSS animations need to be moved to the end for them to start at the same time.)
- An SVG file cleaner is executed to pre-process SVG files so that the main algorithm of ***svgasm*** can be implemented using mainly string operations instead of an XML parser. The animated SVG file output generated using this method does not seem to be further minifiable using *svgcleaner* or *svgo*. *svgo* gives an empty SVG when given the output of ***svgasm***.
- The `<svg>` tag including its attributes (such as `width`, `height` and `viewbox`) in the first SVG file in the sequence of command-line arguments to the ***svgasm*** tool is copied to the output. It is assumed that all frames have the same size and viewport bounds, or all frames are effectively cropped to the size of the first frame. This approach is taken for simplicity in this proof-of-concept implementation.
- The ***svgasm*** tool generates a self-contained output file with a configurable loading text, the content of all frames to be animated and CSS animation styles only. It does not add `<object>` or `<script>` tags.

## Note on loading text in output

- By default, the output of ***svgasm*** contains a centered loading text ("Loading ..." by default) displayed in sans serif font, which is especially useful for a large SVG file. The text element and style cost a total of 147 bytes with default `loadingtext` and `idprefix`. The loading text in output can be turned off with argument `-l ''`.
- The loading text can only be seen as the file is loading in Chrome and Firefox. It cannot be seen in Safari, which waits until SVG is completely loaded before displaying any elements.
- The loading text cannot be viewed on GitHub since GitHub replace the view with its own spinning wheel image when an SVG file is loading. Therefore, a preview of the loading text is provided as follows.

![Loading text preview](readme/loading_text.svg)

## To-dos

Reduce output file size
- Use all alphanumeric characters in prefix for element IDs.
- De-duplication of repeated elements across frames (needs parser).

Reduce output generation time
- Parallelize cleaner and tracer execution for input files on multiple threads.

Improve terminal I/O
- Command line argument input sanitization.
- Argument option `-q` for quietness to suppress standard error output.
- Progress bar for processing input files.

Add more functionalities
- A GUI front-end for the *svgasm* tool.
- Support frame extraction from animated PNG files.
- Assembler to combine two or more SVG animations in sequence.
- Disassembler for SVG animation created with *svgasm*.
- Add transition animation during frame play with configurable keyframes percentage and timing function.
