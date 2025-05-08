# Insulin Pump Simulator

A final project for COMP3004w25, with Prof. Vojislav D Radonjic.

Submitted April 26, 2025

demo: https://youtu.be/um0isKPMlNc



## About

This project simulates the testing conditions for, and the behavior of, an insulin pump. This is a very similar pump functionality to the t:slim insulin pump.



## Project Folder Organization

This project has two main folders:

- `documentation`: Contains all the documentation required for planning this project, including use cases, state diagrams, sequence diagrams, the main UML diagram, and a requirement traceability matrix. All components are in their respectively named folders. (Note: you will need the draw.io app/site to open drawio files, some PDF program to open PDFs, a browser to watch our youtube demo, and VSCode with the PlantUML extension to open our UML.)
- `src`: The project code for the simulator itself. This is written in Qt C++, intended to be run on the COMP3004-F24 course VM. Open the 'src.pro' file and press the run button to run the code.



## Team Members

Adham Sedik (101267731)

Elijah Bruhn (101229879)

Kyle Tu (101269161)

Shailen Chander (101218052)



## Contributions

An X in a column means that the person made contributions to that section of  the project.



### Documentation

| Item                | Adham | Elijah | Kyle | Shailen |
| ------------------- | ----- | ------ | ---- | ------- |
| Use Cases           | x     | x      | x    | x       |
| State Diagrams      |       |        |      | x       |
| Sequence Diagrams   | x     | x      |      |         |
| Traceability Matrix |       |        | x    | x       |
| UML Diagram         | x     |        | x    | x       |



### Code

Everyone got their hands dirty with each section of the codebase on some level, and some sections got rewritten/refactored, hiding old contributions. This only serves as a broad idea of who did what -- this is a list of items and their 'main person/people,' but this is <u>not</u> a list of the *only* people who worked on each item.



| Item                                           | Main Contributor(s) |
| ---------------------------------------------- | ------------------- |
| Simulator functionality + UI controls          | Shailen             |
| Device UI (most Device pages)                  | Elijah              |
| Bolus delivery logic and execution (controlIQ) | Adham, Elijah       |
| Device page switching engine                   | Shailen             |
| Blood glucose graph                            | Shailen             |
| General error handling for Device              | Kyle                |
| Profile creation, editing, deletion + UI       | Shailen, Adham      |
| Log History                                    | Kyle                |
| UI controls + pages                            | Kyle                |
|Manual bolus + UI                               |Adham. Shailen       |

