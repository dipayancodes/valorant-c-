#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

class Player {
private:
    std::string name;
    std::string agent;
    int credits;
    int health;
    int armor;
    std::pair<int, int> position;
    std::string weapon;

public:
    Player(std::string _name, std::string _agent, int _credits)
        : name(_name), agent(_agent), credits(_credits), health(100), armor(0), position(std::make_pair(0, 0)), weapon("Pistol") {}

    void takeDamage(int damage) {
        int totalDamage = damage - armor;
        if (totalDamage > 0) {
            health -= totalDamage;
        }
    }

    void move(int x, int y) {
        position.first += x;
        position.second += y;
    }

    void shoot(Player& target) {
        int damage;
        if (weapon == "Pistol") {
            damage = rand() % 11 + 10;
        } else if (weapon == "Rifle") {
            damage = rand() % 11 + 20;
        } else if (weapon == "Shotgun") {
            damage = rand() % 11 + 5;
        } else {
            damage = 0;
        }

        target.takeDamage(damage);
        std::cout << name << " shot " << target.getName() << " for " << damage << " damage." << std::endl;
    }

    void purchaseWeapon() {
        std::cout << "Available weapons:" << std::endl;
        std::vector<std::string> weapons = {"Pistol", "Rifle", "Shotgun"};
        for (size_t i = 0; i < weapons.size(); ++i) {
            std::cout << i + 1 << ". " << weapons[i] << std::endl;
        }
        int choice;
        std::cout << "Enter the weapon number you want to purchase: ";
        std::cin >> choice;

        if (choice < 1 || choice > weapons.size()) {
            std::cout << "Invalid choice!" << std::endl;
        } else {
            std::string weapon = weapons[choice - 1];
            int weaponCost = (weapon == "Rifle") ? 100 : 50; // Adjust the costs as needed

            if (credits >= weaponCost) {
                this->weapon = weapon;
                credits -= weaponCost;
                std::cout << "You purchased a " << weapon << "." << std::endl;
            } else {
                std::cout << "Not enough credits to purchase the weapon." << std::endl;
            }
        }
    }

    void shootAttack(Player& target) {
        shoot(target);
    }

    void moveAttack(Player& target) {
        std::pair<int, int> moveDirection = {0, 0};
        int choice = rand() % 4;
        if (choice == 0) {
            moveDirection.second = -1;
        } else if (choice == 1) {
            moveDirection.second = 1;
        } else if (choice == 2) {
            moveDirection.first = -1;
        } else if (choice == 3) {
            moveDirection.first = 1;
        }

        move(moveDirection.first, moveDirection.second);
        std::cout << name << " moved to (" << position.first << ", " << position.second << ")." << std::endl;
    }

    void purchaseWeaponAttack(Player& target) {
        purchaseWeapon();
    }

    void opponentAttack(Player& target) {
        int choice = rand() % 3;
        if (choice == 0) {
            moveAttack(target);
        } else if (choice == 1) {
            shootAttack(target);
        } else if (choice == 2) {
            purchaseWeaponAttack(target);
        }
    }

    std::string getName() const {
        return name;
    }

    int getHealth() const {
        return health;
    }

    std::pair<int, int> getPosition() const {
        return position;
    }

    std::string getWeapon() const {
        return weapon;
    }
    
    std::string getAgent() const {
        return agent;
    }

    int getCredits() const {
        return credits;
    }

};

class Game {
private:
    std::string mode;
    std::vector<std::string> agents;
    std::vector<std::string> weapons;

public:
    Game(std::string _mode) : mode(_mode) {
        agents = load_data("agents.txt");
        weapons = load_data("weapons.txt");
    }

    std::vector<std::string> load_data(std::string filename) {
        std::ifstream file(filename);
        std::vector<std::string> data;
        std::string line;
        while (std::getline(file, line)) {
            data.push_back(line);
        }
        return data;
    }

    void start() {
        std::string player1_name;
        std::cout << "Enter your name: ";
        std::cin >> player1_name;
        std::string agent = chooseAgent();
        int credits = (mode == "unrated") ? 200 : 0;
        Player player1(player1_name, agent, credits);

        // Generate a random player for the opponent
        std::string opponentAgent = agents[rand() % agents.size()];
        Player player2("Opponent", opponentAgent, 200);

        // Game loop
        while (player1.getHealth() > 0 && player2.getHealth() > 0) {
            std::cout << "\n=== Turn: " << player1.getName() << " ===" << std::endl;
            displayStats(player1, player2);
            takeTurn(player1, player2);

            if (player2.getHealth() <= 0) {
                break;
            }

            player2.opponentAttack(player1);
        }

        // Game Over
        if (player1.getHealth() > 0) {
            std::cout << "\nCongratulations! " << player1.getName() << " wins!" << std::endl;
        } else {
            std::cout << "\nYou lost! Better luck next time." << std::endl;
        }
    }

    std::string chooseAgent() {
        std::cout << "Choose an agent:" << std::endl;
        for (size_t i = 0; i < agents.size(); ++i) {
            std::cout << i + 1 << ". " << agents[i] << std::endl;
        }
        int choice;
        std::cout << "Enter the agent number: ";
        std::cin >> choice;
        return agents[choice - 1];
    }

    void takeTurn(Player& currentPlayer, Player& otherPlayer) {
        std::cout << "\nChoose an action:" << std::endl;
        std::cout << "1. Move" << std::endl;
        std::cout << "2. Shoot" << std::endl;
        std::cout << "3. Purchase Weapon" << std::endl;

        int choice;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (choice == 1) {
            currentPlayer.moveAttack(otherPlayer);
        } else if (choice == 2) {
            currentPlayer.shootAttack(otherPlayer);
        } else if (choice == 3) {
            currentPlayer.purchaseWeaponAttack(otherPlayer);
        } else {
            std::cout << "Invalid choice!" << std::endl;
        }
    }

    void displayStats(const Player& player, const Player& opponent) {
        std::cout << "\n" << player.getName() << "'s Stats:" << std::endl;
        std::cout << "Agent: " << player.getAgent() << std::endl;
        std::cout << "Weapon: " << player.getWeapon() << std::endl;
        std::cout << "Credits: " << player.getCredits() << std::endl;
        std::cout << "Health: " << player.getHealth() << std::endl;
        std::cout << "Position: (" << player.getPosition().first << ", " << player.getPosition().second << ")" << std::endl;

        std::cout << "\n" << opponent.getName() << "'s Stats:" << std::endl;
        std::cout << "Agent: " << opponent.getAgent() << std::endl;
        std::cout << "Weapon: " << opponent.getWeapon() << std::endl;
        std::cout << "Health: " << opponent.getHealth() << std::endl;
    }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr))); // Seed the random number generator
    std::cout << "Welcome to Text-Based Valorant!" << std::endl;
    std::cout << "Choose a mode:" << std::endl;
    std::cout << "1. Unrated" << std::endl;
    std::cout << "2. Competitive" << std::endl;
    std::cout << "3. Team Death Match" << std::endl;

    int modeChoice;
    std::cout << "Enter the mode number: ";
    std::cin >> modeChoice;

    std::string mode;
    if (modeChoice == 1) {
        mode = "unrated";
    } else if (modeChoice == 2) {
        mode = "competitive";
    } else if (modeChoice == 3) {
        mode = "team_death_match";
    } else {
        std::cout << "Invalid choice!" << std::endl;
        return 1;
    }

    Game game(mode);
    game.start();

    return 0;
}
