// lunar_lander.cpp

#include <emscripten.h>
#include <emscripten/html5.h>
#include <cmath>
#include <string>
#include <vector>
#include <random>

const int canvas_width = 775;
const int canvas_height = 400;

// Map properties
double ground_height = 30;

// Base properties
const int base_height = 40;
const int base_width = 60;
int base_hp = 100;

// Shop properties
const int shop_height = 20;
const int shop_width = 120;
bool shop_open = false;

// Physics constants
const double gravity = 0.05;



// Ship properties
const double ship_width = 20.0;
const double ship_height = 20.0;
const double ship_air_resistance = 0.015;
const double ship_ground_resistance = 0.03;

// Enemy properties
const double enemy_width = 20;
const double enemy_height = 20;
double enemy_speed = 2.0; // Made variable for potential upgrades
double enemy_damage_to_shield = 10.0; // Variable enemy damage to shield
double enemy_damage_to_base = 10.0;   // Variable enemy damage to base

// Set up random number generator
std::random_device rd;  // Non-deterministic random number generator
std::mt19937 gen(rd()); // Seed the generator
std::normal_distribution<double> normal_dist(5.5, 2);

// Spawn enemies every 10 seconds
double current_time = emscripten_get_now();
double spawn_time = emscripten_get_now() - 1000;
int enemies_to_spawn = 0;
double time_between_enemies = 1000.0;
int random_enemy_qty = 0;

// Timers
double last_block_spawn_time = emscripten_get_now() - 5000; // Initialize so that the first block spawns immediately
double last_group_spawn_time = emscripten_get_now();

// Game variables
int money = 0;        // Placeholder for money
int score = 0;        // Score increments by 10 with each enemy death
int level_number = 1; // Placeholder for level number

// Base class for objects that interact with the environment
class PhysicalObject {
public:
    double x, y;
    double vx, vy;
    double ax, ay;
    double width, height;
    bool is_landed;
    double x_ground_drag;
    double x_air_drag;

    PhysicalObject(double startX, double startY, double objWidth, double objHeight, bool isLanded, double x_ground_drag, double x_air_drag)
        : x(startX), y(startY), vx(0.0), vy(0.0), ax(0.0), ay(0.0), width(objWidth), height(objHeight), is_landed(isLanded), x_ground_drag(x_ground_drag), x_air_drag(x_air_drag) {}

    virtual void update() = 0;
    virtual void draw() const = 0;

    virtual void update_physics() {
        // Apply gravity
        ay += gravity;

        // Apply drag proportional to velocity
        double drag_coefficient = is_landed ? x_ground_drag : x_air_drag;
        ax -= drag_coefficient * vx;

        // Update velocities
        vx += ax;
        vy += ay;

        // Update positions
        x += vx;
        y += vy;

        // Boundary collision
        if (x - width / 2 <= 0) {
            x = width / 2;
            vx = 0.0;
        } else if (x + width / 2 >= canvas_width) {
            x = canvas_width - width / 2;
            vx = 0.0;
        }

        // Ground interaction
        is_landed = false; // Assume not landed

        // Check shop interaction first
        if (y + height / 2 >= canvas_height - ground_height - shop_height) {
            if (x <= shop_width) {
                y = canvas_height - ground_height - shop_height - height / 2;
                is_landed = true;
                vy = 0.0;
            }
        }
        // Check base interaction next
        if (!is_landed && y + height / 2 >= canvas_height - ground_height - base_height) {
            if (shop_width <= x && x <= shop_width + base_width) {
                y = canvas_height - ground_height - base_height - height / 2;
                is_landed = true;
                vy = 0.0;
            }
        }
        // Finally, check ground interaction
        if (!is_landed && y + height / 2 >= canvas_height - ground_height) {
            y = canvas_height - ground_height - height / 2;
            is_landed = true;
            vy = 0.0;
        }
    }
};

// Ship class
class Ship : public PhysicalObject {
public:
    double thrust;
    double thrust_x;
    bool isThrusting;
    bool isThrustLeft;
    bool isThrustRight;
    bool game_over;

    // Rope properties
    double rope_length;
    double rope_angle;
    double rope_angular_velocity;
    double rope_angular_acceleration;
    double rope_damping;

    // Shield properties
    double shield_hp;
    double max_shield_hp; // For display purposes

    Ship(double startX, double startY)
        : PhysicalObject(startX, startY, ship_width, ship_height, false, ship_ground_resistance, ship_air_resistance),
          thrust(-0.12), thrust_x(0.15),
          isThrusting(false), isThrustLeft(false), isThrustRight(false), game_over(false),
          rope_length(50.0), rope_angle(0.1), rope_angular_velocity(0.0),
          rope_angular_acceleration(0.0), rope_damping(0.98),
          shield_hp(20.0), max_shield_hp(20.0) {}

    void update_physics() override {
        PhysicalObject::update_physics(); // Call base class method
        if (is_landed && x < shop_width) {
            shop_open = true;
        }
    }

    void update() override {
        // Reset accelerations
        ax = 0.0;
        ay = 0.0;

        // Apply thrusts
        if (isThrustLeft) {
            ax -= thrust_x;
        }
        if (isThrustRight) {
            ax += thrust_x;
        }
        // Apply vertical thrust
        if (isThrusting) {
            ay += thrust;
        }

        // Update rope physics considering horizontal acceleration
        rope_angular_acceleration = (-gravity / rope_length) * sin(rope_angle) - (ax / rope_length) * cos(rope_angle);
        rope_angular_velocity += rope_angular_acceleration;
        rope_angular_velocity *= rope_damping; // Apply damping
        rope_angle += rope_angular_velocity;

        // Update physics
        update_physics();

        // Open shop if landed on the shop area
        if (is_landed && x <= shop_width) {
            shop_open = true;
        }

        // Close shop if ship takes off from shop
        if (shop_open && !is_landed) {
            shop_open = false;
        }
    }

    void draw() const override {
        // Draw lander
        EM_ASM_({
            var ctx = Module.canvas.getContext('2d');
            ctx.fillStyle = 'gray';
            ctx.beginPath();
            ctx.moveTo($0 - 10, $1 + 10);
            ctx.lineTo($0, $1 - 10);
            ctx.lineTo($0 + 10, $1 + 10);
            ctx.closePath();
            ctx.fill();
        }, x, y);

        if (isThrusting) {
            // Draw thrust flame
            EM_ASM_({
                var ctx = Module.canvas.getContext('2d');
                ctx.fillStyle = 'orange';
                ctx.beginPath();
                ctx.moveTo($0 - 5, $1 + 10);
                ctx.lineTo($0, $1 + 20 + Math.random() * 5);
                ctx.lineTo($0 + 5, $1 + 10);
                ctx.closePath();
                ctx.fill();
            }, x, y);
        }

        // Draw shield if shield_hp > 0
        if (shield_hp > 0) {
            EM_ASM_({
                var ctx = Module.canvas.getContext('2d');
                ctx.strokeStyle = 'blue';
                ctx.lineWidth = 3;
                ctx.beginPath();
                ctx.arc($0, $1, 20, 0, 2 * Math.PI);
                ctx.stroke();
            }, x, y);
        }

        // Draw rope
        double end_x = x + rope_length * sin(rope_angle);
        double end_y = y + height / 2 + rope_length * cos(rope_angle);

        EM_ASM_({
            var ctx = Module.canvas.getContext('2d');
            // Draw the rope
            ctx.strokeStyle = 'red';
            ctx.lineWidth = 3;
            ctx.beginPath();
            ctx.moveTo($0, $1);
            ctx.lineTo($2, $3);
            ctx.stroke();
        }, x, y + height / 2, end_x, end_y);
    }
};
Ship player_ship(canvas_width / 2, canvas_height - ground_height - ship_height / 2);

// Block class
const double rect_width = 20;
const double rect_height = 10;
const double rect_air_resistance = 0.015;
const double rect_ground_resistance = 0.03;
class Block : public PhysicalObject {
public:
    int hp;
    bool attached;
    int cooldown_counter;
    int damage; // Damage dealt to enemies

    Block(double startX, double startY)
        : PhysicalObject(startX, startY, rect_width, rect_height, false, rect_ground_resistance, rect_air_resistance),
          hp(40), attached(false), cooldown_counter(0), damage(10) {}

    void update() override {
        // Decrement cooldown counter if it's active
        if (cooldown_counter > 0) {
            cooldown_counter--;
        }
        if (attached) {
            // Update position to be at the end of the rope
            x = player_ship.x + player_ship.rope_length * sin(player_ship.rope_angle);
            y = player_ship.y + player_ship.height / 2 + player_ship.rope_length * cos(player_ship.rope_angle);
        } else {
            // Reset accelerations
            ax = 0.0;
            ay = 0.0;
            // Update physics
            update_physics();
        }
    }

    void draw() const override {
        EM_ASM_({
            var ctx = Module.canvas.getContext('2d');
            ctx.fillStyle = 'yellow';
            ctx.fillRect($0 - $2 / 2, $1 - $3 / 2, $2, $3);
            // Draw hp text
            ctx.fillStyle = 'black';
            ctx.font = '12px Arial';
            ctx.fillText($4.toString(), $0 - 5, $1 + 5);
        }, x, y, width, height, hp);
    }
};

std::vector<Block> blocks;
Block* attached_block = nullptr;

// Enemy class
class Enemy {
public:
    double x, y;
    double speed;
    double hp;

    Enemy(double startX, double startY, double enemySpeed)
        : x(startX), y(startY), speed(enemySpeed), hp(20.0) {}

    void move() {
        x -= speed;
    }

    bool checkCollision(double other_x, double other_y, double other_width, double other_height) {
        return x - enemy_width / 2 < other_x + other_width / 2 &&
               x + enemy_width / 2 > other_x - other_width / 2 &&
               y - enemy_height / 2 < other_y + other_height / 2 &&
               y + enemy_height / 2 > other_y - other_height / 2;
    }

    bool reachedBase() {
        return x - enemy_width / 2 <= base_width + shop_width;
    }
};

std::vector<Enemy> enemies;

// Global ship instance

// Variables for upgrades
double spike_damage_increase = 0.0; // Additional damage dealt by blocks to enemies

void restart_game() {
    // Reset ship
    player_ship = Ship(canvas_width / 2, canvas_height - ground_height - ship_height / 2);

    // Reset blocks
    blocks.clear();
    attached_block = nullptr;

    // Reset enemies
    enemies.clear();

    // Reset base hp
    base_hp = 100;

    // Reset block spawn timer
    last_block_spawn_time = emscripten_get_now() - 5000; // Reset so that a block spawns immediately

    // Reset upgrades
    spike_damage_increase = 0.0;
    enemy_damage_to_shield = 10.0;
    enemy_damage_to_base = 10.0;

    // Reset game variables
    money = 0;
    score = 0;
    level_number = 1;
}

// Function to draw text on the canvas (corrected)
void draw_text(const std::string& text, int x, int y, const char* color = "white", int fontSize = 20) {
    EM_ASM_({
        var ctx = Module.canvas.getContext('2d');
        ctx.fillStyle = UTF8ToString($3);
        ctx.font = $4 + 'px Arial';
        ctx.fillText(UTF8ToString($0), $1, $2);
    }, text.c_str(), x, y, color, fontSize);
}

// Function to draw an enemy
void draw_enemy(double x, double y) {
    EM_ASM_({
        var ctx = Module.canvas.getContext('2d');
        ctx.fillStyle = 'red';
        ctx.fillRect($0 - $2 / 2, $1 - $3 / 2, $2, $3);
    }, x, y, enemy_width, enemy_height);
}

// Update enemies
void update_enemies() {
    for (auto enemy_it = enemies.begin(); enemy_it != enemies.end();) {
        enemy_it->move();

        // Check collision with the ship
        if (enemy_it->checkCollision(player_ship.x, player_ship.y, player_ship.width, player_ship.height)) {
            if (player_ship.shield_hp > 0) {
                player_ship.shield_hp -= enemy_damage_to_shield;
                enemy_it->hp -= enemy_damage_to_shield;
                if (player_ship.shield_hp < 0) player_ship.shield_hp = 0;

                // Check if enemy is destroyed
                if (enemy_it->hp <= 0) {
                    enemy_it = enemies.erase(enemy_it);
                    score += 10; // Increment score
                    continue;
                }
            } else {
                // If no shield, game over or reduce ship HP (not implemented)
                player_ship.game_over = true;
            }
        }

        // Check if enemy reached the base
        if (enemy_it->reachedBase()) {
            base_hp -= enemy_damage_to_base;
            if (base_hp <= 0) {
                player_ship.game_over = true;
            }
            enemy_it = enemies.erase(enemy_it);
            continue;
        }

        // Check collision with blocks
        bool enemy_erased = false;
        for (auto block_it = blocks.begin(); block_it != blocks.end();) {
            if (enemy_it->checkCollision(block_it->x, block_it->y, block_it->width, block_it->height)) {
                double total_damage = block_it->damage + spike_damage_increase;
                enemy_it->hp -= total_damage;
                block_it->hp -= enemy_damage_to_shield; // Assuming enemies deal damage to blocks as well

                if (block_it->hp <= 0) {
                    if (block_it->attached) {
                        // If the block is attached, detach it
                        if (attached_block == &(*block_it)) {
                            attached_block = nullptr;
                        }
                    }
                    block_it = blocks.erase(block_it);
                } else {
                    ++block_it;
                }

                if (enemy_it->hp <= 0) {
                    enemy_it = enemies.erase(enemy_it);
                    score += 10; // Increment score
                    enemy_erased = true;
                    break; // Enemy is removed, break out of block loop
                } else {
                    break; // Enemy is still alive, but no need to check other blocks
                }
            } else {
                ++block_it;
            }
        }
        if (!enemy_erased) {
            ++enemy_it;
        }
    }
}

// Draw all enemies
void draw_enemies() {
    for (const auto& enemy : enemies) {
        draw_enemy(enemy.x, enemy.y);
    }
}

// Draw the shop interface
void draw_shop() {
    // Semi-transparent background
    EM_ASM_({
        var ctx = Module.canvas.getContext('2d');
        ctx.fillStyle = 'rgba(0, 0, 0, 0.5)';
        ctx.fillRect(0, 0, $0, $1);
    }, canvas_width, canvas_height);

    // Shop box
    int shop_box_width = 400;
    int shop_box_height = 300;
    int shop_box_x = (canvas_width - shop_box_width) / 2;
    int shop_box_y = (canvas_height - shop_box_height) / 2;

    EM_ASM_({
        var ctx = Module.canvas.getContext('2d');
        ctx.fillStyle = 'white';
        ctx.fillRect($0, $1, $2, $3);
    }, shop_box_x, shop_box_y, shop_box_width, shop_box_height);

    // Draw buttons
    int button_width = 150;
    int button_height = 40;
    int button_x = (canvas_width - button_width) / 2;
    int button_y = shop_box_y + 50;

    // Shield Upgrade Button
    EM_ASM_({
        var ctx = Module.canvas.getContext('2d');
        ctx.fillStyle = 'lightblue';
        ctx.fillRect($0, $1, $2, $3);
        ctx.fillStyle = 'black';
        ctx.font = '20px Arial';
        ctx.fillText('Shield Upgrade', $0 + 10, $1 + 25);
    }, button_x, button_y, button_width, button_height);

    // Spike Damage Upgrade Button
    EM_ASM_({
        var ctx = Module.canvas.getContext('2d');
        ctx.fillStyle = 'lightgreen';
        ctx.fillRect($0, $1 + 50, $2, $3);
        ctx.fillStyle = 'black';
        ctx.font = '20px Arial';
        ctx.fillText('Spike Damage', $0 + 10, $1 + 75);
    }, button_x, button_y, button_width, button_height);

    // Ship Fuel Upgrade Button
    EM_ASM_({
        var ctx = Module.canvas.getContext('2d');
        ctx.fillStyle = 'lightgrey';
        ctx.fillRect($0, $1 + 100, $2, $3);
        ctx.fillStyle = 'black';
        ctx.font = '20px Arial';
        ctx.fillText('Ship Fuel', $0 + 10, $1 + 125);
    }, button_x, button_y, button_width, button_height);

    // Shop Title
    draw_text("Shop", canvas_width / 2 - 30, shop_box_y + 30, "black", 30);
}

void draw_game_over_screen() {
    // Semi-transparent background overlay
    EM_ASM_({
        var ctx = Module.canvas.getContext('2d');
        ctx.fillStyle = 'rgba(0, 0, 0, 0.7)';
        ctx.fillRect(0, 0, $0, $1);
    }, canvas_width, canvas_height);

    // "Game Over" text
    draw_text("Game Over", canvas_width / 2 - 100, canvas_height / 2 - 100, "white", 50);

    // Display final score and level
    draw_text("Final Score: " + std::to_string(score), canvas_width / 2 - 80, canvas_height / 2 - 30, "white", 30);
    draw_text("Final Level: " + std::to_string(level_number), canvas_width / 2 - 80, canvas_height / 2 + 10, "white", 30);

    // Draw Restart button
    int button_width = 200;
    int button_height = 50;
    int button_x = (canvas_width - button_width) / 2;
    int button_y = canvas_height / 2 + 60;

    EM_ASM_({
        var ctx = Module.canvas.getContext('2d');
        ctx.fillStyle = 'red';
        ctx.fillRect($0, $1, $2, $3);
        ctx.fillStyle = 'white';
        ctx.font = '30px Arial';
        ctx.fillText('Restart', $0 + 50, $1 + 35);
    }, button_x, button_y, button_width, button_height);
}

// Update game state
void update() {
    if (player_ship.game_over) {
        return;
    }

    player_ship.update();

    // Compute the rope's end point
    double rope_end_x = player_ship.x + player_ship.rope_length * sin(player_ship.rope_angle);
    double rope_end_y = player_ship.y + player_ship.height / 2 + player_ship.rope_length * cos(player_ship.rope_angle);

    // Static variables to hold the previous position
    static double rope_end_x_prev = rope_end_x;
    static double rope_end_y_prev = rope_end_y;

    // Compute the rope end point velocities
    double rope_end_vx = rope_end_x - rope_end_x_prev;
    double rope_end_vy = rope_end_y - rope_end_y_prev;

    if (attached_block == nullptr) {
        // Check for collision between rope end and blocks
        for (auto& block : blocks) {
            if (!block.attached && block.cooldown_counter == 0) {
                if (rope_end_x >= block.x - block.width / 2 &&
                    rope_end_x <= block.x + block.width / 2 &&
                    rope_end_y >= block.y - block.height / 2 &&
                    rope_end_y <= block.y + block.height / 2) {
                    block.attached = true;
                    attached_block = &block;
                    // Set the block's velocities to match the rope end point velocities
                    block.vx = rope_end_vx;
                    block.vy = rope_end_vy;
                    break;
                }
            }
        }
    } else {
        // Update the position of the attached block to follow the rope end
        attached_block->x = rope_end_x;
        attached_block->y = rope_end_y;
        // Update the velocities
        attached_block->vx = rope_end_vx;
        attached_block->vy = rope_end_vy;
    }

    // Update the previous rope end position
    rope_end_x_prev = rope_end_x;
    rope_end_y_prev = rope_end_y;

    // Update blocks
    for (auto& block : blocks) {
        block.update();
    }

    // Spawn spikes
    double current_time = emscripten_get_now();
    if (current_time - last_block_spawn_time >= 5000.0) { // 5000 milliseconds = 5 seconds
        if (blocks.size() < 5) {
            blocks.emplace_back(shop_width + base_width / 2, canvas_height - ground_height - base_height - rect_height);
            last_block_spawn_time = current_time;
        }
    }

    // Spawn enemies
    // if (current_time - last_group_spawn_time >= 10000.0) {
    //     if (random_enemy_qty == 0) {
    //         // Generate a random value from the normal distribution and clamp it to the range [1, 10]
    //         random_enemy_qty = std::clamp(static_cast<int>(std::round(normal_dist(gen))), 1, 10);
    //         last_group_spawn_time = emscripten_get_now();
    //     }
    //     if (current_time - spawn_time > time_between_enemies && random_enemy_qty > 0) {
    //         enemies.emplace_back(canvas_width - enemy_width / 2, canvas_height - ground_height - enemy_height / 2, enemy_speed);
    //         spawn_time = emscripten_get_now();
    //         random_enemy_qty -= 1;
    //     }
    // }

    // Update enemies
    update_enemies();
}

// Render game elements
void render() {
    // Clear canvas
    EM_ASM({
        var ctx = Module.canvas.getContext('2d');
        ctx.fillStyle = 'black'; // Background color
        ctx.fillRect(0, 0, $0, $1);
    }, canvas_width, canvas_height);

    if (player_ship.game_over) {
        // Draw the game over screen
        draw_game_over_screen();
    } else {
        // Draw ground
        EM_ASM_({
            var ctx = Module.canvas.getContext('2d');
            ctx.fillStyle = 'green';
            ctx.fillRect(0, $0 - $2, $1, $2);
        }, canvas_height, canvas_width, ground_height);

        // Draw shop
        EM_ASM_({
            var ctx = Module.canvas.getContext('2d');
            ctx.fillStyle = 'grey';
            ctx.fillRect(0, $0, $1, $2);
        }, canvas_height - ground_height - shop_height, shop_width, shop_height);

        // Draw base
        EM_ASM_({
            var ctx = Module.canvas.getContext('2d');
            ctx.fillStyle = 'white';
            ctx.fillRect($1, $0, $2, $3);
        }, canvas_height - ground_height - base_height, shop_width, base_width, base_height);

        // Draw ship
        player_ship.draw();

        // Draw blocks
        for (const auto& block : blocks) {
            block.draw();
        }

        // Draw enemies
        draw_enemies();

        // Draw Text
        draw_text("Base HP: " + std::to_string(base_hp), 10, 30, "white");
        draw_text("Shield HP: " + std::to_string(static_cast<int>(player_ship.shield_hp)), 10, 60, "white");
        draw_text("Money: " + std::to_string(money), 10, 90, "white");
        draw_text("Score: " + std::to_string(score), 10, 120, "white");
        draw_text("Level: " + std::to_string(level_number), 10, 150, "white");

        // Draw shop if open
        if (shop_open) {
            draw_shop();
        }
    }
}

// Main game loop
void main_loop() {
    update();
    render();
}

// Input handling
EM_BOOL keydown_callback(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
    std::string key = e->key;

    if (key == "ArrowUp") {
        player_ship.isThrusting = true;
    } else if (key == "ArrowLeft") {
        player_ship.isThrustLeft = true;
    } else if (key == "ArrowRight") {
        player_ship.isThrustRight = true;
    } else if (key == " " || key == "Space") { // Spacebar to release the block
        if (attached_block != nullptr) {
            attached_block->attached = false;
            // Start cooldown counter
            attached_block->cooldown_counter = 60; // 1 second at 60 FPS
             // Compute the rope's end point velocities
            double rope_end_vx = attached_block->vx;
            double rope_end_vy = attached_block->vy;
            // Set the block's velocity to match the rope's end point velocity
            attached_block->vx = rope_end_vx;
            attached_block->vy = rope_end_vy;
            attached_block = nullptr;
        }
    } else if (key == "r") {
        restart_game();
    } else if (key == "t") {
        enemies.emplace_back(canvas_width - enemy_width / 2, canvas_height - ground_height - enemy_height / 2, enemy_speed);
    }
    return EM_TRUE;
}
EM_BOOL keyup_callback(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
    std::string key = e->key;

    if (key == "ArrowUp") {
        player_ship.isThrusting = false;
    } else if (key == "ArrowLeft") {
        player_ship.isThrustLeft = false;
    } else if (key == "ArrowRight") {
        player_ship.isThrustRight = false;
    }
    return EM_TRUE;
}
EM_BOOL mousedown_callback(int eventType, const EmscriptenMouseEvent* e, void* userData) {
    if (player_ship.game_over) {
        int mouseX = e->clientX;
        int mouseY = e->clientY;

        // Restart button dimensions (same as in draw_game_over_screen)
        int button_width = 200;
        int button_height = 50;
        int button_x = (canvas_width - button_width) / 2;
        int button_y = canvas_height / 2 + 60;

        // Check if the restart button was clicked
        if (mouseX >= button_x && mouseX <= button_x + button_width &&
            mouseY >= button_y && mouseY <= button_y + button_height) {
            restart_game();
            return EM_TRUE;
        }
    } else if (shop_open) {
        // Existing shop click handling code...
        int mouseX = e->clientX;
        int mouseY = e->clientY;

        // Shop button dimensions
        int button_width = 150;
        int button_height = 40;
        int button_x = (canvas_width - button_width) / 2;
        int button_y_start = (canvas_height - 3 * button_height) / 2;

        // Check which button was clicked
        if (mouseX >= button_x && mouseX <= button_x + button_width) {
            // Shield Upgrade Button
            if (mouseY >= button_y_start && mouseY <= button_y_start + button_height) {
                // Upgrade shield
                player_ship.max_shield_hp += 10;
                player_ship.shield_hp = player_ship.max_shield_hp;
                shop_open = false; // Close shop after purchase
                return EM_TRUE;
            }
            // Spike Damage Upgrade Button
            else if (mouseY >= button_y_start + button_height + 10 && mouseY <= button_y_start + 2 * button_height + 10) {
                // Upgrade spike damage
                spike_damage_increase += 10.0;
                shop_open = false; // Close shop after purchase
                return EM_TRUE;
            }
            // Ship Fuel Upgrade Button (Not implemented)
            else if (mouseY >= button_y_start + 2 * (button_height + 10) && mouseY <= button_y_start + 3 * button_height + 20) {
                // For now, do nothing
                shop_open = false; // Close shop after purchase
                return EM_TRUE;
            }
        }
    }
    return EM_FALSE;
}

// Main function
int main() {
    // Set up canvas size
    EM_ASM({
        var canvas = Module.canvas;
        canvas.width = $0;
        canvas.height = $1;
    }, canvas_width, canvas_height);

    // Set up input handling
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, keydown_callback);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, keyup_callback);

    // Set up mouse click handling
    emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_TRUE, mousedown_callback);

    // Start main loop
    emscripten_set_main_loop(main_loop, 0, 1);

    return 0;
}