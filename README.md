# Go Game Development

## Project Description
This repository contains the source code for a Go game (Weiqi) developed as a Windows Desktop Application using C++. The game adheres to traditional Go rules, offering a playable interface where users can engage in strategic gameplay. The project aims to provide an educational tool for learning Go while also serving as a coding exercise in game development.

## Table of Contents
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Game Rules](#game-rules)
- [Technologies Used](#technologies-used)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Features
- **Interactive GUI:** A user-friendly interface allowing players to easily place stones and view game status.
- **Turn Management:** Proper turn-taking mechanics for both players (Black and White).
- **Move Validation:** Ensures all moves adhere to Go rules, including checks for self-capture and forbidden moves.
- **Scoring System:** Automatically calculates scores based on captured stones and territory control at the end of the game.
- **Game State Management:** Allows players to pass their turn or resign, with appropriate notifications for the game's conclusion.
- **Capture Tracking:** Keeps track of captured stones for both players throughout the game.

## Installation
To run the Go game application, follow these steps:

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/yourusername/go-game.git
   cd go-game
   ```

2. **Open the Project in Visual Studio:**
   - Open `Go_Game.sln` in Visual Studio.

3. **Build the Project:**
   - Select the appropriate build configuration (Debug/Release).
   - Build the project to restore dependencies and compile the code.

4. **Run the Application:**
   - After building, run the application from Visual Studio or execute the generated `.exe` file in the output directory.

## Usage
- **Starting the Game:** Launch the application, and the game board will be displayed.
- **Placing Stones:** Click on an intersection to place your stone. The Black player goes first.
- **Passing or Resigning:** Use the "Pass" button to skip your turn or the "Resign" button to end the game.
- **End of Game:** The game will conclude when both players pass consecutively or one player resigns. Scores will be calculated automatically.

## Game Rules
Refer to the following key rules to understand how to play Go:
- The game is played on a 19x19 grid.
- Players alternate placing stones on the intersections.
- A stone may not be placed on an intersection that results in self-capture.
- The game ends when both players pass consecutively or one player resigns.
- The winner is determined by counting the number of living stones and the territory controlled.

For a more comprehensive overview of Go rules, visit the [Rules of Go](https://en.wikipedia.org/wiki/Rules_of_Go).

## Technologies Used
- C++
- Windows API for GUI
- Visual Studio for development

## Contributing
Contributions to this project are welcome! If you would like to contribute, please follow these steps:
1. Fork the repository.
2. Create a new branch (`git checkout -b feature/YourFeature`).
3. Make your changes and commit them (`git commit -m 'Add some feature'`).
4. Push to the branch (`git push origin feature/YourFeature`).
5. Create a new Pull Request.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact
For any questions or feedback, please reach out to:
- **Name:** Samiul Hasan Shimanto
- **Email:** samiulhasan6730@gmail.com

---

Thank you for your interest in the Go Game project!
```
