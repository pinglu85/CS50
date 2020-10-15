from cs50 import get_int


def main():
    height = get_height()
    draw_pyramid(height, height)


# Prompt user for the pyramid height
# between 1 and 8 inclusive
def get_height():
    while True:
        h = get_int('Height: ')
        if h >= 1 and h <= 8:
            break
    return h


# Draw pyramid
def draw_pyramid(row, h):
    if row == 0:
        return
    draw_pyramid(row - 1, h)
    # Draw leading spaces
    space_count = h - row
    print(' ' * space_count, end='')
    # Draw first half of bricks
    print('#' * row, end='')
    # Draw gap
    print('  ', end='')
    # Draw second half of bricks
    print('#' * row)


if __name__ == '__main__':
    main()