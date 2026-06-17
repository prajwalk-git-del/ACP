#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define WIDTH 60
#define HEIGHT 20
#define MAX_SHAPES 100

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Define shape type 
typedef enum {
    SHAPE_LINE = 1,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;

// Define structs for each shape's properties
typedef struct {
    int x1, y1, x2, y2;
} LineData;

typedef struct {
    int x, y, w, h;
} RectData;

typedef struct {
    int cx, cy, r;
} CircleData;

typedef struct {
    int x1, y1, x2, y2, x3, y3;
} TriData;

// Shape wrapper
typedef struct {
    ShapeType type;
    union {
        LineData line;
        RectData rect;
        CircleData circle;
        TriData tri;
    } data;
} Shape;

// Global array of shapes
Shape shapes[MAX_SHAPES];
int num_shapes = 0;

// Canvas definition
char canvas[HEIGHT][WIDTH];

// Initialize canvas with background character
void clear_canvas() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

// Drawing helper: Line (DDA-based)
void draw_line(int x1, int y1, int x2, int y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double steps = fabs(dx) > fabs(dy) ? fabs(dx) : fabs(dy);
    if (steps < 1) {
        if (x1 >= 0 && x1 < WIDTH && y1 >= 0 && y1 < HEIGHT) {
            canvas[y1][x1] = '*';
        }
        return;
    }
    double xInc = dx / steps;
    double yInc = dy / steps;
    double x = x1;
    double y = y1;
    for (int i = 0; i <= steps; i++) {
        int px = (int)round(x);
        int py = (int)round(y);
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            canvas[py][px] = '*';
        }
        x += xInc;
        y += yInc;
    }
}

// Drawing helper: Rectangle (outline)
void draw_rectangle(int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;
    for (int i = 0; i < w; i++) {
        int px = x + i;
        if (px >= 0 && px < WIDTH) {
            if (y >= 0 && y < HEIGHT) canvas[y][px] = '*';
            if (y + h - 1 >= 0 && y + h - 1 < HEIGHT) canvas[y + h - 1][px] = '*';
        }
    }
    for (int i = 0; i < h; i++) {
        int py = y + i;
        if (py >= 0 && py < HEIGHT) {
            if (x >= 0 && x < WIDTH) canvas[py][x] = '*';
            if (x + w - 1 >= 0 && x + w - 1 < WIDTH) canvas[py][x + w - 1] = '*';
        }
    }
}

// Drawing helper: Circle (polar step)
void draw_circle(int cx, int cy, int r) {
    if (r <= 0) return;
    // Step angle based on radius to avoid gaps
    double step = 1.0 / (double)r;
    for (double theta = 0; theta < 2.0 * M_PI; theta += step) {
        int px = (int)round(cx + r * cos(theta));
        int py = (int)round(cy + r * sin(theta));
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            canvas[py][px] = '*';
        }
    }
}

// Drawing helper: Triangle
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}

// Render all active shapes on canvas
void render_canvas() {
    clear_canvas();
    for (int i = 0; i < num_shapes; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case SHAPE_LINE:
                draw_line(s.data.line.x1, s.data.line.y1, s.data.line.x2, s.data.line.y2);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(s.data.rect.x, s.data.rect.y, s.data.rect.w, s.data.rect.h);
                break;
            case SHAPE_CIRCLE:
                draw_circle(s.data.circle.cx, s.data.circle.cy, s.data.circle.r);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(s.data.tri.x1, s.data.tri.y1, s.data.tri.x2, s.data.tri.y2, s.data.tri.x3, s.data.tri.y3);
                break;
        }
    }
}

// Display the canvas with clean border
void display_canvas() {
    render_canvas();
    
    // Top border
    printf(" +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");

    // Canvas contents
    for (int y = 0; y < HEIGHT; y++) {
        printf(" |");
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", canvas[y][x]);
        }
        printf("|\n");
    }

    // Bottom border
    printf(" +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");
}

// Print shapes list
void list_shapes() {
    if (num_shapes == 0) {
        printf("No shapes in the list.\n");
        return;
    }
    printf("\n=== Current Shapes ===\n");
    for (int i = 0; i < num_shapes; i++) {
        printf("[%d] ", i + 1);
        switch (shapes[i].type) {
            case SHAPE_LINE:
                printf("Line: (%d, %d) to (%d, %d)\n", 
                       shapes[i].data.line.x1, shapes[i].data.line.y1, 
                       shapes[i].data.line.x2, shapes[i].data.line.y2);
                break;
            case SHAPE_RECTANGLE:
                printf("Rectangle: top-left (%d, %d), size %dx%d\n", 
                       shapes[i].data.rect.x, shapes[i].data.rect.y, 
                       shapes[i].data.rect.w, shapes[i].data.rect.h);
                break;
            case SHAPE_CIRCLE:
                printf("Circle: center (%d, %d), radius %d\n", 
                       shapes[i].data.circle.cx, shapes[i].data.circle.cy, 
                       shapes[i].data.circle.r);
                break;
            case SHAPE_TRIANGLE:
                printf("Triangle: vertices (%d, %d), (%d, %d), (%d, %d)\n", 
                       shapes[i].data.tri.x1, shapes[i].data.tri.y1, 
                       shapes[i].data.tri.x2, shapes[i].data.tri.y2, 
                       shapes[i].data.tri.x3, shapes[i].data.tri.y3);
                break;
        }
    }
}

// Add shape helper
void add_shape() {
    if (num_shapes >= MAX_SHAPES) {
        printf("Maximum shape limit reached!\n");
        return;
    }

    printf("\nSelect Shape Type to Add:\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    printf("Enter choice (1-4): ");
    int choice;
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n'); // clear buffer
        return;
    }

    Shape s;
    s.type = (ShapeType)choice;

    switch (s.type) {
        case SHAPE_LINE:
            printf("Enter x1 y1 x2 y2: ");
            if (scanf("%d %d %d %d", &s.data.line.x1, &s.data.line.y1, &s.data.line.x2, &s.data.line.y2) != 4) {
                printf("Invalid coordinates.\n");
                while (getchar() != '\n');
                return;
            }
            break;
        case SHAPE_RECTANGLE:
            printf("Enter x y (top-left) width height: ");
            if (scanf("%d %d %d %d", &s.data.rect.x, &s.data.rect.y, &s.data.rect.w, &s.data.rect.h) != 4) {
                printf("Invalid rectangle values.\n");
                while (getchar() != '\n');
                return;
            }
            break;
        case SHAPE_CIRCLE:
            printf("Enter cx cy (center) radius: ");
            if (scanf("%d %d %d", &s.data.circle.cx, &s.data.circle.cy, &s.data.circle.r) != 3) {
                printf("Invalid circle values.\n");
                while (getchar() != '\n');
                return;
            }
            break;
        case SHAPE_TRIANGLE:
            printf("Enter x1 y1 x2 y2 x3 y3: ");
            if (scanf("%d %d %d %d %d %d", 
                      &s.data.tri.x1, &s.data.tri.y1, 
                      &s.data.tri.x2, &s.data.tri.y2, 
                      &s.data.tri.x3, &s.data.tri.y3) != 6) {
                printf("Invalid triangle coordinates.\n");
                while (getchar() != '\n');
                return;
            }
            break;
        default:
            printf("Invalid shape type.\n");
            return;
    }

    shapes[num_shapes++] = s;
    printf("Shape added successfully!\n");
}

// Delete shape helper
void delete_shape() {
    if (num_shapes == 0) {
        printf("No shapes to delete.\n");
        return;
    }
    list_shapes();
    printf("Enter the index of the shape to delete (1-%d): ", num_shapes);
    int idx;
    if (scanf("%d", &idx) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    idx--; // convert to 0-based index
    if (idx < 0 || idx >= num_shapes) {
        printf("Invalid index.\n");
        return;
    }

    for (int i = idx; i < num_shapes - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    num_shapes--;
    printf("Shape deleted successfully!\n");
}

// Modify shape helper
void modify_shape() {
    if (num_shapes == 0) {
        printf("No shapes to modify.\n");
        return;
    }
    list_shapes();
    printf("Enter the index of the shape to modify (1-%d): ", num_shapes);
    int idx;
    if (scanf("%d", &idx) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    idx--; // convert to 0-based index
    if (idx < 0 || idx >= num_shapes) {
        printf("Invalid index.\n");
        return;
    }

    Shape *s = &shapes[idx];
    printf("Modifying shape [%d]...\n", idx + 1);

    switch (s->type) {
        case SHAPE_LINE:
            printf("Current Line: (%d, %d) to (%d, %d)\n", s->data.line.x1, s->data.line.y1, s->data.line.x2, s->data.line.y2);
            printf("Enter new x1 y1 x2 y2: ");
            if (scanf("%d %d %d %d", &s->data.line.x1, &s->data.line.y1, &s->data.line.x2, &s->data.line.y2) != 4) {
                printf("Invalid coordinates.\n");
                while (getchar() != '\n');
                return;
            }
            break;
        case SHAPE_RECTANGLE:
            printf("Current Rectangle: top-left (%d, %d), size %dx%d\n", s->data.rect.x, s->data.rect.y, s->data.rect.w, s->data.rect.h);
            printf("Enter new x y width height: ");
            if (scanf("%d %d %d %d", &s->data.rect.x, &s->data.rect.y, &s->data.rect.w, &s->data.rect.h) != 4) {
                printf("Invalid rectangle values.\n");
                while (getchar() != '\n');
                return;
            }
            break;
        case SHAPE_CIRCLE:
            printf("Current Circle: center (%d, %d), radius %d\n", s->data.circle.cx, s->data.circle.cy, s->data.circle.r);
            printf("Enter new cx cy radius: ");
            if (scanf("%d %d %d", &s->data.circle.cx, &s->data.circle.cy, &s->data.circle.r) != 3) {
                printf("Invalid circle values.\n");
                while (getchar() != '\n');
                return;
            }
            break;
        case SHAPE_TRIANGLE:
            printf("Current Triangle: (%d, %d), (%d, %d), (%d, %d)\n", 
                   s->data.tri.x1, s->data.tri.y1, 
                   s->data.tri.x2, s->data.tri.y2, 
                   s->data.tri.x3, s->data.tri.y3);
            printf("Enter new x1 y1 x2 y2 x3 y3: ");
            if (scanf("%d %d %d %d %d %d", 
                      &s->data.tri.x1, &s->data.tri.y1, 
                      &s->data.tri.x2, &s->data.tri.y2, 
                      &s->data.tri.x3, &s->data.tri.y3) != 6) {
                printf("Invalid triangle coordinates.\n");
                while (getchar() != '\n');
                return;
            }
            break;
    }
    printf("Shape modified successfully!\n");
}

int main() {
    int choice;
    printf("=========================================\n");
    printf("  ASCII 2D Graphics Editor (Static Array)\n");
    printf("  Canvas Size: %d x %d | Background: '_'\n", WIDTH, HEIGHT);
    printf("=========================================\n");

    while (1) {
        printf("\nMenu:\n");
        printf("1. Add Object\n");
        printf("2. Delete Object\n");
        printf("3. Modify Object\n");
        printf("4. Display Picture\n");
        printf("5. List Objects\n");
        printf("6. Exit\n");
        printf("Enter command: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid selection. Please enter a number.\n");
            while (getchar() != '\n'); // clear buffer
            continue;
        }

        switch (choice) {
            case 1:
                add_shape();
                break;
            case 2:
                delete_shape();
                break;
            case 3:
                modify_shape();
                break;
            case 4:
                display_canvas();
                break;
            case 5:
                list_shapes();
                break;
            case 6:
                printf("Exiting... Goodbye!\n");
                return 0;
            default:
                printf("Unknown menu option %d.\n", choice);
        }
    }
}
