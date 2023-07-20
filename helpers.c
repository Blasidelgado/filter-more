#include "helpers.h"
// Include need libraries in order to run the program correctly
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

#define EDGES 9

typedef struct
{
    int mod_red;
    int mod_green;
    int mod_blue;
} MOD_PIXEL;

RGBTRIPLE average(int count, ...);

MOD_PIXEL calculate_gx(RGBTRIPLE top_left, RGBTRIPLE top, RGBTRIPLE top_right, RGBTRIPLE left, RGBTRIPLE center, RGBTRIPLE right,
                       RGBTRIPLE bottom_left, RGBTRIPLE bottom, RGBTRIPLE bottom_right);
MOD_PIXEL calculate_gy(RGBTRIPLE top_left, RGBTRIPLE top, RGBTRIPLE top_right, RGBTRIPLE left, RGBTRIPLE center,
                       RGBTRIPLE right, RGBTRIPLE bottom_left, RGBTRIPLE bottom, RGBTRIPLE bottom_right);
RGBTRIPLE sobel_filter(MOD_PIXEL gx, MOD_PIXEL gy);

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    int average;

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            average = round((float)(image[row][col].rgbtRed + image[row][col].rgbtGreen + image[row][col].rgbtBlue) / 3);
            image[row][col].rgbtRed = average;
            image[row][col].rgbtGreen = average;
            image[row][col].rgbtBlue = average;
        }
    };

    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE temp;
    int lastSwap;
    for (int row = 0; row < height; row++)
    {
        // Check for parity or imparity of row width
        if (width % 2 == 1)
        {
            lastSwap = width / 2;
        }
        else
        {
            lastSwap = width / 2 - 1;
        }
        for (int col = 0; col <= lastSwap; col++)
        {
            // Swap places to create reflection effect
            temp = image[row][col];
            image[row][col] = image[row][(width - 1) - col];
            image[row][(width - 1) - col] = temp;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Create a copy image to fill with blurred RGBs
    RGBTRIPLE blurred_image[height][width];

    // Start scanning the image
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            // Check for edges and fill the image copy
            if (row == 0) // Check if we are at the top of the image
            {
                if (col == 0) // Check for top left corner
                {
                    blurred_image[row][col] = average(4, image[row][col], image[row][col + 1], image[row + 1][col], image[row + 1][col + 1]);
                }
                else if (col == width - 1) // Check for top right corner
                {
                    blurred_image[row][col] = average(4, image[row][col], image[row][col - 1], image[row + 1][col], image[row + 1][col - 1]);
                }
                else
                {
                    blurred_image[row][col] = average(6, image[row][col], image[row][col - 1], image[row][col + 1], image[row + 1][col - 1],
                                                      image[row + 1][col], image[row + 1][col + 1]);
                }
            }
            else if (row == height - 1) // Check if we are at the bottom of the image
            {
                if (col == 0) // Check for bottom left corner
                {
                    blurred_image[row][col] = average(4, image[row][col], image[row][col + 1], image[row - 1][col], image[row - 1][col + 1]);
                }
                else if (col == width - 1) // Check for bottom right corner
                {
                    blurred_image[row][col] = average(4, image[row][col], image[row][col - 1], image[row - 1][col], image[row - 1][col - 1]);
                }
                else
                {
                    blurred_image[row][col] = average(6, image[row][col], image[row][col - 1], image[row][col + 1], image[row - 1][col - 1],
                                                      image[row - 1][col], image[row - 1][col + 1]);
                }
            }
            else if (col == 0) // Check if we are at the leftmost column of the image
            {
                blurred_image[row][col] = average(6, image[row][col], image[row - 1][col], image[row + 1][col], image[row - 1][col + 1],
                                                  image[row][col + 1], image[row + 1][col + 1]);
            }
            else if (col == width - 1) // CHeck if we are at the rightmost column of the image
            {
                blurred_image[row][col] = average(6, image[row][col], image[row - 1][col], image[row + 1][col], image[row - 1][col - 1],
                                                  image[row][col - 1], image[row + 1][col - 1]);
            }
            else
            {
                blurred_image[row][col] = average(9, image[row][col], image[row - 1][col - 1], image[row - 1][col], image[row - 1][col + 1],
                                                  image[row][col - 1], image[row][col + 1], image[row + 1][col - 1], image[row + 1][col], image[row + 1][col + 1]);
            }
        }
    }

    // Iterate the blurred image to assign values to the original image
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            image[row][col] = blurred_image[row][col];
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Create a copy image to fill with blurred RGBs
    RGBTRIPLE edged_image[height][width];

    // Create black pixels for edges
    RGBTRIPLE black_pixel = { 0, 0, 0 };

    // Start scanning the image
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
// Check for edges and fill the image copy
            if (row == 0) // Check if we are at the top of the image
            {
                if (col == 0) // Check for top left corner
                {
                    edged_image[row][col] = sobel_filter(
                                                calculate_gx(black_pixel, black_pixel, black_pixel, black_pixel, image[row][col], image[row][col + 1], black_pixel,
                                                        image[row + 1][col], image[row + 1][col + 1]),
                                                calculate_gy(black_pixel, black_pixel, black_pixel, black_pixel, image[row][col], image[row][col + 1], black_pixel,
                                                        image[row + 1][col], image[row + 1][col + 1])
                                            );
                }
                else if (col == width - 1) // Check for top right corner
                {
                    edged_image[row][col] = sobel_filter(
                                                calculate_gx(black_pixel, black_pixel, black_pixel, image[row][col - 1], image[row][col], black_pixel, image[row + 1][col - 1],
                                                        image[row + 1][col], black_pixel),
                                                calculate_gy(black_pixel, black_pixel, black_pixel, image[row][col - 1], image[row][col], black_pixel, image[row + 1][col - 1],
                                                        image[row + 1][col], black_pixel)
                                            );
                }
                else
                {
                    edged_image[row][col] = sobel_filter(
                                                calculate_gx(black_pixel, black_pixel, black_pixel, image[row][col - 1], image[row][col], image[row][col + 1],
                                                        image[row + 1][col - 1], image[row + 1][col], image[row + 1][col + 1]),
                                                calculate_gy(black_pixel, black_pixel, black_pixel, image[row][col - 1], image[row][col], image[row][col + 1],
                                                        image[row + 1][col - 1], image[row + 1][col], image[row + 1][col + 1])
                                            );
                }
            }
            else if (row == height - 1) // Check if we are at the bottom of the image
            {
                if (col == 0) // Check for bottom left corner
                {
                    edged_image[row][col] = sobel_filter(
                                                calculate_gx(black_pixel, image[row - 1][col], image[row - 1][col + 1], black_pixel, image[row][col], image[row][col + 1],
                                                        black_pixel, black_pixel, black_pixel),
                                                calculate_gy(black_pixel, image[row - 1][col], image[row - 1][col + 1], black_pixel, image[row][col], image[row][col + 1],
                                                        black_pixel, black_pixel, black_pixel)
                                            );
                }
                else if (col == width - 1) // Check for bottom right corner
                {
                    edged_image[row][col] = sobel_filter(
                                                calculate_gx(image[row - 1][col - 1], image[row - 1][col], black_pixel, image[row][col - 1], image[row][col], black_pixel,
                                                        black_pixel, black_pixel, black_pixel),
                                                calculate_gy(image[row - 1][col - 1], image[row - 1][col], black_pixel, image[row][col - 1], image[row][col], black_pixel,
                                                        black_pixel, black_pixel, black_pixel)
                                            );
                }
                else
                {
                    edged_image[row][col] = sobel_filter(
                                                calculate_gx(image[row - 1][col - 1], image[row - 1][col], image[row - 1][col + 1], image[row][col - 1], image[row][col],
                                                        image[row][col + 1], black_pixel, black_pixel, black_pixel),
                                                calculate_gy(image[row - 1][col - 1], image[row - 1][col], image[row - 1][col + 1], image[row][col - 1], image[row][col],
                                                        image[row][col + 1], black_pixel, black_pixel, black_pixel)
                                            );
                }
            }
            else if (col == 0) // Check if we are at the leftmost column of the image
            {
                edged_image[row][col] = sobel_filter(
                                            calculate_gx(black_pixel, image[row - 1][col], image[row - 1][col + 1], black_pixel, image[row][col], image[row][col + 1],
                                                    black_pixel, image[row + 1][col], image[row + 1][col + 1]),
                                            calculate_gy(black_pixel, image[row - 1][col], image[row - 1][col + 1], black_pixel, image[row][col], image[row][col + 1],
                                                    black_pixel, image[row + 1][col], image[row + 1][col + 1])
                                        );
            }
            else if (col == width - 1) // Check if we are at the rightmost column of the image
            {
                edged_image[row][col] = sobel_filter(
                                            calculate_gx(image[row - 1][col - 1], image[row - 1][col], black_pixel, image[row][col - 1], image[row][col], black_pixel,
                                                    image[row + 1][col - 1], image[row + 1][col], black_pixel),
                                            calculate_gy(image[row - 1][col - 1], image[row - 1][col], black_pixel, image[row][col - 1], image[row][col], black_pixel,
                                                    image[row + 1][col - 1], image[row + 1][col], black_pixel)
                                        );
            }
            else
            {
                edged_image[row][col] = sobel_filter(
                                            calculate_gx(image[row - 1][col - 1], image[row - 1][col], image[row - 1][col + 1], image[row][col - 1], image[row][col],
                                                    image[row][col + 1], image[row + 1][col - 1], image[row + 1][col], image[row + 1][col + 1]),
                                            calculate_gy(image[row - 1][col - 1], image[row - 1][col], image[row - 1][col + 1], image[row][col - 1], image[row][col],
                                                    image[row][col + 1], image[row + 1][col - 1], image[row + 1][col], image[row + 1][col + 1])
                                        );
            }
        }
    }

    // Iterate the blurred image to assign values to the original image
    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            image[row][col] = edged_image[row][col];
        }
    }
    return;
};

RGBTRIPLE average(int count, ...)
{
    int total_red = 0, total_green = 0, total_blue = 0;
    int average_red = 0, average_green = 0, average_blue = 0;
    RGBTRIPLE current, blurred;

    // Create list with given arguments
    va_list args;

    // Initialize the list
    va_start(args, count);

    // Iterate the list and calculate average of RBGs
    for (int i = 0; i < count; i++)
    {
        current = va_arg(args, RGBTRIPLE);
        total_red += current.rgbtRed;
        total_green += current.rgbtGreen;
        total_blue += current.rgbtBlue;
    }

    // Clean up the list
    va_end(args);

    // Take the final sum and calculate average
    average_red = round((float) total_red / count);
    average_green = round((float) total_green / count);
    average_blue = round((float) total_blue / count);

    // Fill the new RGB blurred pixel
    blurred.rgbtRed = average_red;
    blurred.rgbtGreen = average_green;
    blurred.rgbtBlue = average_blue;

    return blurred;
};

MOD_PIXEL calculate_gx(RGBTRIPLE top_left, RGBTRIPLE top, RGBTRIPLE top_right, RGBTRIPLE left, RGBTRIPLE center, RGBTRIPLE right,
                       RGBTRIPLE bottom_left, RGBTRIPLE bottom, RGBTRIPLE bottom_right)
{
    int red = 0, green = 0, blue = 0;
    RGBTRIPLE pixels[EDGES] = {top_left, top, top_right, left, center, right, bottom_left, bottom, bottom_right};
    MOD_PIXEL x;

    for (int i = 0; i < EDGES; i++)
    {
        switch (i) // Check pixel and take all its channel values
        {
            case 0:
            case 6:
                red += pixels[i].rgbtRed * (-1);
                green += pixels[i].rgbtGreen * (-1);
                blue += pixels[i].rgbtBlue * (-1);
                break;
            case 2:
            case 8:
                red += pixels[i].rgbtRed;
                green += pixels[i].rgbtGreen;
                blue += pixels[i].rgbtBlue;
                break;
            case 3:
                red += pixels[i].rgbtRed * (-2);
                green += pixels[i].rgbtGreen * (-2);
                blue += pixels[i].rgbtBlue * (-2);
                break;
            case 5:
                red += pixels[i].rgbtRed * 2;
                green += pixels[i].rgbtGreen * 2;
                blue += pixels[i].rgbtBlue * 2;
                break;
            default:
                break;
        }
    }

    // Fill the new modified RGB
    x.mod_red = red;
    x.mod_green = green;
    x.mod_blue = blue;

    return x;
};

MOD_PIXEL calculate_gy(RGBTRIPLE top_left, RGBTRIPLE top, RGBTRIPLE top_right, RGBTRIPLE left, RGBTRIPLE center, RGBTRIPLE right,
                       RGBTRIPLE bottom_left, RGBTRIPLE bottom, RGBTRIPLE bottom_right)
{
    int red = 0, green = 0, blue = 0;
    RGBTRIPLE pixels[EDGES] = {top_left, top, top_right, left, center, right, bottom_left, bottom, bottom_right};
    MOD_PIXEL y;

    for (int i = 0; i < EDGES; i++)
    {
        switch (i) // Check pixel and take all its channel values
        {
            case 0:
            case 2:
                red += pixels[i].rgbtRed * (-1);
                green += pixels[i].rgbtGreen * (-1);
                blue += pixels[i].rgbtBlue * (-1);
                break;
            case 6:
            case 8:
                red += pixels[i].rgbtRed;
                green += pixels[i].rgbtGreen;
                blue += pixels[i].rgbtBlue;
                break;
            case 1:
                red += pixels[i].rgbtRed * (-2);
                green += pixels[i].rgbtGreen * (-2);
                blue += pixels[i].rgbtBlue * (-2);
                break;
            case 7:
                red += pixels[i].rgbtRed * 2;
                green += pixels[i].rgbtGreen * 2;
                blue += pixels[i].rgbtBlue * 2;
                break;
            default:
                break;
        }
    }

    // Fill the new modified RGB
    y.mod_red = red;
    y.mod_green = green;
    y.mod_blue = blue;

    return y;
};

RGBTRIPLE sobel_filter(MOD_PIXEL gx, MOD_PIXEL gy)
{
    RGBTRIPLE edged;

    // Make sure to apply final sobel filter algorithm and round the number
    int total_red = round((float) sqrt(pow(gx.mod_red, 2) + pow(gy.mod_red, 2)));
    int total_green = round((float) sqrt(pow(gx.mod_green, 2) + pow(gy.mod_green, 2)));
    int total_blue = round((float) sqrt(pow(gx.mod_blue, 2) + pow(gy.mod_blue, 2)));

    // Assign values capped at max possible RGB value
    edged.rgbtRed = total_red > 255 ? 255 : total_red;
    edged.rgbtGreen = total_green > 255 ? 255 : total_green;
    edged.rgbtBlue = total_blue > 255 ? 255 : total_blue;

    return edged;
};
