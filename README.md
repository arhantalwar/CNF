# CNF

Idea implementation from http://users.ece.cmu.edu/~adrian/projects/validation/validation.pdf 
Also would like to thank https://github.com/tsoding/ for streaming, Got me really inspired to do this as well.

![thumbnail](./assets/thumbnail.png)
(Generated using CNF)

## QUICK START

Dependencies

- [raylib](https://www.raylib.com/)

## USAGE

You can use CNF to generate 'Computer Generated Artwork'.
Basically the ```productions``` file have the Grammar in CNF.
It reads from it and produces a string which is then evaluated and then mapped to screen for every pixel giving us this art.

There are parts marked in as "TWEAKS" so that you can tweak the values and get different kinds of outputs.

Example:

```c
img_grid[i][j].c = (Color){
    .r = scaled * 10/2,
        .g = scaled * 10/8,
        .b = scaled * 10/16,
        .a = 255,
        // tweak's
        // You can multiply r, g, b with random values to get different colors
        // multiply with randf(start, end) to get distored pixel effect
};

```

Incase if you liked some image, then you can save the expression in the ```production_output2``` file and can read from it later.
You can also write an expression of your own in the ```production_output2```.
(Use the grammar correctly)

```c
    fp_output = fopen("./production_output2", "r");
    // tweak's
    // Change ./production_output to ./production_output2 to parse a string that you have generated using the grammar.
```

To generate static images.

```console
$ gcc main.c -o main -lraylib -lm
$ ./main 7
```

![Static Image](./assets/output.png)

To generate animated images pass '-f' flag.

```console
$ gcc main.c -o main -lraylib -lm
$ ./main 7 -f
```

<img src="./assets/output.gif" alt="Animated GIF" width="500">

you can take screenshot of the image you like by pressing the 's' key while the image is shown.
