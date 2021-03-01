# *svgasm*

[![Sponsor][img_sponsor]][sponsor] &nbsp; [![Build status][img_build_status]][build_status] &nbsp; [![Language grade: C/C++][img_lgtm]][lgtm] &nbsp; [![License][img_license]][license]

[img_build_status]: https://github.com/tomkwok/svgasm/workflows/svgasm/badge.svg
[build_status]: https://github.com/tomkwok/svgasm/actions?query=branch%3Amaster

[img_lgtm]: https://img.shields.io/lgtm/grade/cpp/g/tomkwok/svgasm.svg?logo=lgtm&logoWidth=18
[lgtm]: https://lgtm.com/projects/g/tomkwok/svgasm/latest/files/

[img_license]: https://img.shields.io/github/license/tomkwok/svgasm?color=green&cacheSeconds=3600
[license]: LICENSE.md

[img_sponsor]: readme/badge_sponsor_animation.svg
[sponsor]: https://github.com/sponsors/tomkwok

[***svgasm***](https://github.com/tomkwok/svgasm) is a proof-of-concept SVG assembler to generate a self-contained animated SVG file from multiple still SVG files with CSS keyframes animation to play frames in sequence. Steps listed in reverse order of execution:

- Produces single animated SVG file that is viewable in Chrome, Safari, Firefox, Edge and IE 10+.
- Executes SVG cleaner [*svgcleaner*](https://github.com/RazrFalcon/svgcleaner) (by default) or [*svgo*](https://github.com/svg/svgo) to minify each SVG file.
- Executes bitmap tracer [*potrace*](http://potrace.sourceforge.net/) (by default) or [*autotrace*](https://github.com/autotrace/autotrace), or [*primitive*](https://github.com/fogleman/primitive) to convert raster image input to SVG.
- Executes image processor [*graphicsmagick*](http://www.graphicsmagick.org) (by default) or [*imagemagick*](https://imagemagick.org/) to convert GIF animation to frames.

![Flow diagram of svgasm from animated GIF to animated SVG](readme/svgasm.1.svg)

## Examples

### SVG animation from single GIF using tracer

If the two versions of animation below are out of sync, try reloading this page without refreshing by clicking [here](https://github.com/tomkwok/svgasm).

<details open><summary><b>Show evolution of life GIF example</b></summary>
<p></p>
<p>Note that colors are inverted with <code>gm convert -negate</code> to obtain black-on-white images before tracing with <em>potrace</em>. The SVG output is hence black-on-transparent but restyled to white-on-black to match the input GIF.</p>
<p><code>
svgasm -t 'gm convert -negate "%s" pgm:- | mkbitmap -x -f 1 -s 1 - -o - | potrace -t 0.4 --svg -o -' -s 'svg {background-color: black} path {fill: white}' examples/_evolution_of_life.gif &gt; examples/evolution_of_life.svg</code>
</p>
<table>
	<tr>
		<th>Input GIF (6,843 KiB)</th>
		<th>Output SVG (3,490 KiB → 1,394 KiB gzipped)</th>
	</tr>
	<tr>
		<td width="50%">
			<img src="examples/_evolution_of_life.gif" width="100%">
		</td>
		<td width="50%">
			<img src="examples/evolution_of_life.svg" width="100%">
		</td>
	</tr>
</table>
</details>

<details open><summary><b>Show rotating cross GIF example</b></summary>
<p></p>
<p><code>
svgasm examples/_rotating_cross.gif > examples/rotating_cross.svg</code>
 (with <em>potrace</em> as tracer by default)</p>
<table>
	<tr>
		<th>Input GIF (665 KiB)</th>
		<th>Output SVG (462 KiB → 119 KiB gzipped)</th>
	</tr>
	<tr>
		<td width="50%">
			<img src="examples/_rotating_cross.gif" width="100%">
		</td>
		<td width="50%">
			<img src="examples/rotating_cross.svg" width="100%">
		</td>
	</tr>
</table>
</details>

<details><summary><b>Show infinity spiral GIF example</b></summary>
<p></p>
<p><code>
svgasm -t 'mkbitmap -t 0.4 -s 1 "%s" -o - | potrace -t 4 --svg -o -' -s 'svg {background-color: white}' examples/_infinity_spiral.gif &gt; examples/infinity_spiral.svg</code>
</p>
<table>
	<tr>
		<th>Input GIF (349 KiB)</th>
		<th>Output SVG (715 KiB → 285 KiB gzipped)</th>
	</tr>
	<tr>
		<td width="50%">
			<img src="examples/_infinity_spiral.gif" width="100%">
		</td>
		<td width="50%">
			<img src="examples/infinity_spiral.svg" width="100%">
		</td>
	</tr>
</table>
</details>

<details><summary><b>Show NSFW cartoon GIF example</b></summary>
<p></p>
<p><code>
svgasm -t 'mkbitmap -x -t 0.44 -s 1 "%s" -o - | potrace --svg -o -' -s 'svg {background-color: #ddd}' examples/_mickey_mouse_nsfw.gif &gt; examples/mickey_mouse_nsfw.svg</code>
</p>
<table>
	<tr>
		<th>Input GIF (912 KiB)</th>
		<th>Output SVG (1,164 KiB → 473 KiB gzipped)</th>
	</tr>
	<tr>
		<td width="50%">
			<img src="examples/_mickey_mouse_nsfw.gif" width="100%">
		</td>
		<td width="50%">
			<img src="examples/mickey_mouse_nsfw.svg" width="100%">
		</td>
	</tr>
</table>
</details>

Note that *potrace* [only natively handles two-valued images](http://potrace.sourceforge.net/faq.html#features). The color values in the SVG output can be specified with extra CSS styles definitions in the `-s` argument to the ***svgasm*** tool as in the command for the evolution of life GIF example. The colors do not have to be black or white or colors in grayscale used in the above examples.

All of the above examples traced with *potrace* can also be traced with *autotrace* with less configuration but also less accurate (sometimes funky) shapes in results, which can be viewed in [examples/autotrace\_results.ipynb](examples/autotrace_results.ipynb).

Use of multiple colors in SVG output is supported natively in *autotrace* but gradients are hard for any tracer.

<details open><summary><b>Show Spongebob GIF example</b></summary>
<p></p>
<p><code>
svgasm -t 'autotrace --output-format svg --color-count 64 --despeckle-level 10 --despeckle-tightness 0.8 --remove-adjacent-corners "%s"' examples/_spongebob.gif &gt; examples/spongebob.svg</code>
</p>
<table>
	<tr>
		<th>Input GIF (426 KiB)</th>
		<th>Output SVG (888 KiB → 259 KiB gzipped)</th>
	</tr>
	<tr>
		<td width="50%">
			<img src="examples/_spongebob.gif" width="100%">
		</td>
		<td width="50%">
			<img src="examples/spongebob.svg" width="100%">
		</td>
	</tr>
</table>
</details>

<details open><summary><b>Show anime GIF example</b></summary>
<p></p>
<p><code>
svgasm -t 'autotrace --output-format svg --color-count 120 --despeckle-level 16 --despeckle-tightness 1.5 --tangent-surround 1 --remove-adjacent-corners "%s"' -s 'svg {background-color: gray}' examples/_anime.gif &gt; examples/anime.svg</code>
</p>
<table>
	<tr>
		<th>Input GIF (1,616 KiB)</th>
		<th>Output SVG (1,268 KiB → 496 KiB gzipped)</th>
	</tr>
	<tr>
		<td width="50%">
			<img src="examples/_anime.gif" width="100%">
		</td>
		<td width="50%">
			<img src="examples/anime.svg" width="100%">
		</td>
	</tr>
</table>
</details>

While *primitive* is not a tracer, it can be used for raster-to-vector conversion with aesthetically pleasing results.

<details open><summary><b>Show lions GIF example</b></summary>
<p></p>
<p><code>
svgasm -t 'gm convert "%s" png:- | primitive -i - -o - -n 250' examples/_lions.gif &gt; examples/lions.svg</code>
</p>
<table>
	<tr>
		<th>Input GIF (2,180 KiB)</th>
		<th>Output SVG (673 KiB → 193 KiB gzipped)</th>
	</tr>
	<tr>
		<td width="50%">
			<img src="examples/_lions.gif" width="100%">
		</td>
		<td width="50%">
			<img src="examples/lions.svg" width="100%">
		</td>
	</tr>
</table>
</details>

### SVG animation from multiple still SVGs

<details open><summary><b>Show build status badge example</b></summary>
<p></p>
<p>An example of a 2-fps 2-frame animated build status badge for this project is generated using <strong><em>svgasm</em></strong> from 2 <a href="https://docs.github.com/en/actions/managing-workflow-runs/adding-a-workflow-status-badge">GitHub Workflows status badge</a> SVG files.</p>
<p><img src="examples/badge_animation.svg" alt="Build status badge animation example"></p>
</details>

<details open><summary><b>Show calendar plot example</b></summary>
<p></p>
<p>A worked example of a 1-fps 2-frame animated calendar plot in <a href="examples/calplot_animation.ipynb">examples/calplot_animation.ipynb</a> (Jupyter Notebook) is generated using <a href="https://github.com/tomkwok/calplot">calplot</a> and <strong><em>svgasm</em></strong>.</p>
<p><a href="examples/calplot_animation.ipynb"><img src="examples/calplot_animation.svg" alt="Calplot animation example"></a></p>
</details>

<details open><summary><b>Show contour plot example</b></summary>
<p></p>
<p>While the above two examples can be created with CSS animation with some effort without the help of <strong><em>svgasm</em></strong>, the following example is not so easy. The following example is a 30-fps 41-frame animated contour plot of <a href="https://en.wikipedia.org/wiki/F-score">F<sub>β</sub> score</a> from a sequence of 41 plots pre-generated using <a href="https://github.com/matplotlib/matplotlib">matplotlib</a> animated with <strong><em>svgasm</em></strong>.</p>
<p><img src="examples/contour_f_beta_animation.svg" alt="Contour plot animation example"></p>
</details>

Notably, ***svgasm*** does not use JavaScript in SVG output. (An alternative approach would be to re-program plot generation with a JavaScript library like [D3.js](https://github.com/d3/d3-contour) to have the browser generate the values of text, paths, gradients, etc.)


## Usage

```
svgasm [options] infilepath...

Options:
  -d <delaysecs>     animation time delay in seconds  (default: 0.1)
  -o <outfilepath>   path to SVG animation output file or - for stdout  (default: -)
  -p <idprefix>      prefix added to element IDs  (default: _)
  -i <itercount>     animation iteration count  (default: infinite)
  -e <endframe>      index of frame to stop at in last iteration if not infinite  (default: -1)
  -l <loadingtext>   loading text in output or '' to turn off  (default: 'Loading ...')
  -s <stylesextra>   extra CSS styles definition in output  (default: '')
  -c <cleanercmd>    command for SVG cleaner with "%s"  (default: 'svgcleaner --multipass -c "%s"')
  -t <tracercmd>     command for tracer for non-SVG still image with "%s"  (default: 'gm convert +matte "%s" ppm:- | potrace --svg -o -')
  -m <magickcmd>     command for magick program for GIF animation with %s  (default: 'gm %s')
  -q                 silence verbose standard error output
  -h                 print help information
```

## Usage examples

- `svgasm -d 2 -i 5 -e 0 -o animation.svg input1.svg input2.svg input3.svg`  
Generates output *animation.svg* from *input1.svg*, *input2.svg* and *input3.svg* that animates with a delay of 2 seconds per frame, iterates 5 times, and stops at the first frame in the last iteration. 
- `svgasm -d 1/30 -l '' intro.jpg frame*.png > animation.svg`  
Generates output *animation.svg* from *intro.jpg* and wild card *frame\*.png* that animates with 30 frames per second, iterates infinitely, and with loading text turned off.
- `svgasm animation1.gif animation2.gif > animation.svg`  
Generates output *animation.svg* from *animation1.gif* and *animation2.gif* that animates with the same time delay and iteration count as the first GIF file.


## Installing on macOS

To install and run a pre-compiled binary of ***svgasm*** on macOS with [Homebrew](https://brew.sh/) installed, run the following commands, which would also install dependencies *svgcleaner*, *potrace* and *graphicsmagick*:

```sh
brew install tomkwok/tap/svgasm
svgasm
```

## Building on macOS or Linux

To clone this repository and build ***svgasm*** with a C++98 complier installed, run the following commands:

```sh
git clone https://github.com/tomkwok/svgasm
cd svgasm/
make
./svgasm
```

***svgasm*** can be executed as a standalone program. However, the only working feature without dependencies is `svgasm [options] input*.svg > output.svg` (with automatic fallback to `cat` as dummy cleaner program).

For more features, install runtime dependencies. An example instruction is provided for [Arch Linux](https://archlinux.org/) as follows:

```sh
sudo pacman -S svgcleaner potrace graphicsmagick
```

To download the example files in this repository with `git-lfs` installed, run the following commands:

```sh
git lfs install
git lfs pull
```

## Installing optional runtime dependencies

- [*svgo*](https://github.com/svg/svgo): running the command `npm install -g svgo` or `yarn global add svgo`
- [*autotrace*](https://github.com/autotrace/autotrace): downloading a release binary from the [release page of autotrace](https://github.com/autotrace/autotrace/releases) or install with a package manager
- [*primitive*](https://github.com/fogleman/primitive): running the command `go get -u github.com/fogleman/primitive`

## Benchmark with different SVG cleaners

***svgasm*** has been tested to work with the following cleaners:

- ***cat*** can be specified as a dummy cleaner program for most SVG files. (In particular, ensure that there are no unnecessary white-spaces in tags.) *cat* is the fallback program if *svgcleaner* is not installed on the system.
- ***svgcleaner*** is the default cleaner program. *svgcleaner* is chosen as the default cleaner because it is generally very fast and produces small output.
- ***svgo*** can be specified as the cleaner program. *svgo* is typically over an order of magnitude slower than *svgcleaner*. Nonetheless, *svgo* can sometimes produce smaller files than *svgcleaner* does.

The following are the results of using ***svgasm*** to produce the 41-frame contour plot animation example above on an Intel Core i5 processor with different cleaners. The size of output after compression with [gzip](https://www.gzip.org/) at the best level `-9` is provided in brackets for reference since SVG files are usually served via HTTP which supports compression.

| Command executed									| Output SVG size (gzip) | Real elapsed time |
|:-------------------------------------------------	| --------------------------- | -----------------:|
| `svgasm -c 'cat "%s"' [...]`						| 3,143 KiB	(1,145 KiB)       | **0.265 s ± 0.006 s** |
| `svgasm -c 'svgcleaner --multipass -c "%s"' [...]`	| 2,232 KiB	(992 KiB)         | 0.844 s ± 0.041 s |
| `svgasm -c 'svgo --multipass -o - "%s"' [...]`		| **2,054 KiB	(884 KiB)**     | 34.839 s ± 0.292 s |

## Benchmark with GraphicsMagick and ImageMagick

GraphicsMagick is a fork of ImageMagick, and it is reportedly faster in [benchmarks](http://www.graphicsmagick.org/benchmarks.html). The following are the results of using ***svgasm*** to produce the 60-frame rotating cross animation example above on an Intel Core i5 processor with GraphicsMagick and ImageMagick specified as the magick command. Note that the magick program is also present in the default tracer command for *potrace*. Identical output is obtained with the two utility programs.

| 				  | Command executed											 | Real elapsed time |
| --------------- | :----------------------------------------------------------- | -----------------:|
| GraphicsMagick  | `svgasm -c 'cat "%s"' -m 'gm %s' -t 'gm convert "%s" [...]'` | **3.205 s ± 0.037 s** |
| ImageMagick     | `svgasm -c 'cat "%s"' -m '%s' -t 'convert "%s" [...]'`		 | 5.493 s ± 0.041 s |

## How ***svgasm*** is implemented

- Refer to an [example](https://stackoverflow.com/questions/48893587/simple-animate-multiple-svgs-in-sequence-like-a-looping-gif) on Stack Overflow of animating a sequence of hand-drawn vector graphics. The principal method employed in the ***svgasm*** tool of playing SVGs in sequence with CSS animation is credited to its author.
- Implementation is fine-tuned in the ***svgasm*** tool to better accommodate browser behavior. In particular, since Chrome renders elements on the fly and starts timing animations on elements as SVG file loads, attention was given to styles placement to prevent flickering in Chrome due to mismatching animation start times for different group elements for frames. See inline comments in [src/svgasm.cpp](src/svgasm.cpp) for details. Note that such flickering is only apparent in an animation with a high fps value. (It is not clear in the [CSS Animations specification](https://www.w3.org/TR/css-animations-1/#animations) whether a style rule is considered resolved or not when the styles are parsed but the elements referenced are not yet in the DOM. The answer is apparently no in Chrome, so CSS animations need to be moved to the end for animation of frames to start at the same time.)
- An SVG file cleaner is executed to pre-process SVG files to ensure that ***svgasm*** can successfully parse input SVG files. For simplicity, ***svgasm*** uses string operations in standard library instead of an XML parser. The animated SVG file output generated does not seem to be further minifiable using *svgcleaner* or *svgo*. Currently, *svgo* gives an empty SVG when given the output of ***svgasm***.
- The `<svg>` tag including its attributes (such as `width`, `height` and `viewbox`) in the first SVG file in the sequence of command-line arguments to the ***svgasm*** tool is copied to the output. It is assumed that all frames have the same size and viewport bounds, or all frames are effectively cropped to the size of the first frame. This approach is taken for simplicity in this proof-of-concept implementation and can be improved.
- The ***svgasm*** tool generates a self-contained output file with a configurable loading text, the content of all SVG frames and CSS animation styles in `<style>`. It does not add any `<object>` or `<script>` tags.

## End-of-animation frame index configurable in SVG output

- The end-of-animation index feature with a default value of -1 in the ***svgasm*** tool is necessary to recreate the behavior in GIF that animation stops at the last frame rather than disappears after loops are completed.
- This feature is extended to accept a configurable end-of-animation frame index *e* specified in argument `-e`, for which -*x* denotes the *x*-th last frame and *0* denotes the first frame. This feature can be turned off with argument `-e ''`  when executing *svgasm* so that the output animation becomes invisible after a predefined number of iterations are completed. This feature has no effect on an animation that loops infinitely.

## Note on CSS animation approach adopted

- While the CSS animation rules to implement the end-of-frame index feature may look simple and obvious in source code, it is tricky to derive them. This is likely due to the fact that CSS rules are imperative. An explicit expression of the mathematical formula for animation delay and iteration count for individual frames are required for CSS animation, which does not natively support chained animations. In contrast, the description of animation in human language as in the help information of ***svgasm*** is procedural. A procedural implementation of animation in JavaScript, which is avoided in this tool, would probably be more straight forward.
- CSS animation as defined in the W3C specification (or similarly in [SMIL](https://www.w3.org/TR/SMIL3/)) is designed for interpolated transition of computed property values rather than for GIF-style time-discretized animation of a sequence of still images adopted in the ***svgasm*** tool. Nonetheless, in order to increase usable fps value of the resulting animation, an approach to be explored is using deep generative networks like [DeepSVG](https://github.com/alexandre01/deepsvg) (2020) to generate interpolated SVG frames to be inserted in between existing SVG frames from input. (Interestingly, all the animated SVG interpolation demos on the DeepSVG [project webpage](https://web.archive.org/web/20201118113845/https://blog.alexandrecarlier.com/deepsvg/) are presented in GIF format. The ***svgasm*** tool would fit in this case as it can be used to create an animated SVG interpolation demo in SVG format.)

## Loading text configurable in SVG output

- By default, the output of ***svgasm*** contains a configurable loading text ("Loading ..." by default) displayed in sans serif font, which is especially useful for a large SVG file since downloading and parsing take some time in a browser. Notably, this is implemented by careful placement of CSS styles and with no JavaScript used.
- The text element and style cost a total of 147 bytes with default `loadingtext` and `idprefix`. The loading text in SVG output can be turned off with argument `-l ''` when executing *svgasm*.
- The loading text can be seen when the file is loading in Chrome, Firefox or Edge. It cannot be seen in Safari or IE 10+, which waits until SVG is completely loaded before displaying any elements.
- The loading text cannot be viewed on GitHub since it is replaced with GitHub's own spinning wheel image when an SVG file is loading. Instead, see custom loading text in action in external file [contour\_itercount_2.svg](https://d33wubrfki0l68.cloudfront.net/a5708d35e08996ebb3e7eb4d26194e97c55ef56e/669a6/plot/iou-vs-f1/contour_itercount_2.svg). Also, a preview of the default loading text is provided as follows.

![Loading text preview](readme/loading_text.svg)

## Origin of the ***svgasm*** tool

- The ***svgasm*** tool was originally created to automate the generation of an SVG animation composed of a relatively high number of SVG files, that is the *matplotlib* contour plot animation example. (SVG animation is [not officially supported in *matplotlib*](https://github.com/matplotlib/matplotlib/pull/4255).) The original program was created in one day, and the flickering issue in Chrome was fixed in the next day. The program at that point can be seen in the first commit of this repository.
- It was found that such animation of moderately complicated SVG content did not perform as horribly as expected in browsers, and it works even when played with a relatively high fps on a computer with no dedicated GPU. Therefore, this project was created to present the ***svgasm*** tool to easily generate time-discretized SVG animation from stills as a viable alternative to raster animation formats like GIF, APNG or video formats. There is potential for more use cases.

## Note on programming language choice

- The implementation of the ***svgasm*** tool started with C++ instead of an interpreted scripting language. This was to avoid having the simple operations in ***svgasm*** for a typical SVG file take a longer time to run than the running time of *svgcleaner*, which is a more technically complex SVG parsing and cleaning tool.
- To maximize compatibility with compilers on older systems, features in C++11 or newer are deliberately avoided. For example, `std::ostringstream` is used instead of `std::to_string` in C++11, and explicit assignment for map values is used instead of initializer lists in C++11. This is to ensure that C++98 support is sufficient to compile the ***svgasm*** tool.

## External links

- An APNG assembler [*apngasm*](https://github.com/apngasm/apngasm). The name of ***svgasm*** is inspired by *apngasm*. It is a sexy name with no pun intended. It can also mean ***svg*** is ***a***we***s***o***m***e as a more platonic interpretation.
- A command-line benchmarking tool [hyperfine](https://github.com/sharkdp/hyperfine) that runs commands at least 10 times and presents time measurements in the format of mean ± sigma (1 standard deviation).
- My proposal of [Tom's rainbow color map](https://tomkwok.com/posts/color-maps/), which is used in the contour plot animation example above.

## To-dos

Reduce output file size
- Use all alphanumeric characters with base-36 encoding in prefix for element IDs.
- De-duplication of repeated elements across frames (needs full parser).

Reduce output generation time
- Parallelize cleaner and tracer execution for input files on multiple threads.

Improve terminal I/O
- Support building and running on Windows (`popen`, `/dev/stdout`, `/dev/null`, `cat`, `getopt.h`, `dirent.h`).
- Command line argument input sanitization.

Add more functionalities
- A GUI front-end for the *svgasm* command-line tool.
- Support configurable output size and viewport bounds.
- Support frame extraction from animated PNG and video files.
- Assembler to combine two or more SVG animations in sequence.
- Disassembler for SVG animation created with *svgasm*.
- Support automatic insertion of interpolated frames to increase usable fps.

## Copyright and licensing

Copyright (C) 2021 tom [at] tomkwok.com. All rights reserved.

- The source code in this repository is licensed under the [Apache License, Version 2.0](LICENSE.md).
- The text in this [README.md](README.md) document is NOT licensed under the Apache License, Version 2.0.
- The copyright claim does not apply to the example GIF images sourced from the internet with unknown original sources. The display of the GIF images and their SVG derivatives generated by ***svgasm*** for the purpose of software demonstration should constitute fair use.

## Support

&nbsp; ❤ &nbsp; [Support via GitHub Sponsors / Bitcoin donation / Buy me a coffee](https://github.com/sponsors/tomkwok)
