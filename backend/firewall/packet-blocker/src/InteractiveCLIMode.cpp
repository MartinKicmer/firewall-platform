#include "../headers/InteractiveCLIMode.h"
#include <cstdio>
#include <cstring>
#include <memory>
#include <ncurses.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>

void InteractiveCLIMode::show() {

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    use_default_colors(); 
    assume_default_colors(COLOR_WHITE, -1);
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);
    init_pair(4, COLOR_WHITE, COLOR_GREEN);
    init_pair(5, COLOR_WHITE, COLOR_WHITE);
    init_pair(6, COLOR_WHITE, COLOR_RED);
    while(!this->stop) {
        erase();
        this->showLayout();
        this->getSelectionIndex('0', '4');
        if(this->selectionIndex == 'q') {
            this->stop = true;
            break;
        }
        if(this->selectionIndex == '\n') this->showMenuLayout();
        refresh();
    }
    endwin();

}


void InteractiveCLIMode::getSelectionIndex(int min,int max) {
    this->lastSelectionIndex = this->selectionIndex;
    this->selectionIndex = getch();
    if (this->selectionIndex >= min && this->selectionIndex <= max) {
        this->selectionIndex = this->selectionIndex - '0'; 
        this->selectionIndex--;
    }
}

void InteractiveCLIMode::showBackOption() {

    const char* art[] = {
        "+------------------------------------------------------------+",
        "|  [ PRESS b ]  T O  G O   B A C K                           |",
        "+------------------------------------------------------------+"
    };
    int artRows = 3;
    int artCols = 64; 
    int startY = (LINES) - 5; 
    int startX = (COLS - artCols) / 2;

    attron(COLOR_PAIR(3) | A_BOLD); 
    for (int i = 0; i < artRows; i++) {
        mvprintw(startY + i, startX, "%s", art[i]);
    }
    attroff(COLOR_PAIR(3) | A_BOLD);
}



void InteractiveCLIMode::showDoneOption() {
    const char* art = {
        "Press [ SPACE ] for confirmation"
    };

    int artRows = 1;
    int artCols = std::strlen(art); 
    int startY = (LINES / 4) - 1; 
    int startX = (COLS - artCols) / 2;

    attron(COLOR_PAIR(4) | A_BOLD); 
    mvprintw(startY, startX, "%s", art);
    attroff(COLOR_PAIR(4) | A_BOLD);

}


void InteractiveCLIMode::showQuitOption() {
     const char* art[] = {
    "+------------------------------------------------------------+",
    "|           [ PRESS q ]  T O   Q U I T   M E N U             |",
    "+------------------------------------------------------------+"
    };
    int artRows = 3;
    int artCols = 64; 
    int startY = (LINES ) - 6; 
    int startX = (COLS - artCols) / 2;

    attron(COLOR_PAIR(6) | A_BOLD); 
    for (int i = 0; i < artRows; i++) {
        mvprintw(startY + i, startX, "%s", art[i]);
    }
    attroff(COLOR_PAIR(6) | A_BOLD);
}

void InteractiveCLIMode::createFirewallRule() {
    std::shared_ptr<Rule> rule = nullptr;
    auto& inputs = this->concreteLayerInputs.at(this->lastSelectionIndex);

    int ID = -1;
    switch (this->lastSelectionIndex) {
        case 0: ID = safeStoi(inputs.at(4), -1); break; 
        case 1: ID = safeStoi(inputs.at(6), -1); break; 
        case 2: ID = safeStoi(inputs.at(4), -1); break; 
        case 3: ID = safeStoi(inputs.at(6), -1); break; 
    }
    if(ID == -1) throw std::runtime_error("Invalid ID");
    switch (this->lastSelectionIndex) {
        case 0: { 
            bool permit = isPermit(inputs.at(2));
            int limit = safeStoi(inputs.at(3), -1);
            rule = std::make_shared<L2Rule>(permit, limit, 
                                            inputs.at(0).empty() ? "none" : inputs.at(0), 
                                            inputs.at(1).empty() ? "none" : inputs.at(1));
            break;
        }
        case 1: { 
            bool permit = isPermit(inputs.at(5));
            std::tuple<std::string,int> srcIP;
            std::tuple<std::string,int> destIP;
            parseIP(srcIP, inputs.at(0));
            parseIP(destIP, inputs.at(1));
            rule = std::make_shared<L3Rule>(
                permit, 
                -1, 
                srcIP, destIP, 
                safeStoi(inputs.at(3), -1), 
                safeStoi(inputs.at(2), -1),   
                safeStoi(inputs.at(4), -1)   
            );
            break;
        }
        case 2: { 
            rule = std::make_shared<L4SimpleRule>(
                isPermit(inputs.at(2)),
                safeStoi(inputs.at(3), -1),
                safeStoi(inputs.at(0), -1), 
                safeStoi(inputs.at(1), -1)  
            );
            break;
        }
       case 3: { 
            bool permit = isPermit(inputs.at(5));
            rule = std::make_shared<L4TcpRule>(
                permit,
                -1, 
                safeStoi(inputs.at(0), -1), 
                safeStoi(inputs.at(1), -1), 
                (uint8_t)safeStoi(inputs.at(2), 0), 
                (short)safeStoi(inputs.at(3), -1),  
                (short)safeStoi(inputs.at(4), -1)   
            );
            break;
        }
    }
    if (rule) {
        this->parsedFilterRule = std::make_shared<FilterRule>(rule,ID);
    } else {
        throw std::runtime_error("Could not parse rule in interactive mode in the final stage\n");
    }
}


void InteractiveCLIMode::moveIndex(int& index,int ch,int max) {
    if (ch == KEY_UP && index > 0) index--;
    if (ch == KEY_DOWN && index < max - 1) index++;
}
void InteractiveCLIMode::showConcreteLayerOption() {
    int formIndex = 0; 
    auto& options = this->concreteLayerOptions.at(this->lastSelectionIndex);
    int totalFields = options.size();

    while(!this->stop) {
        erase();
        this->showFrame(COLS, LINES); 
        
        attron(A_BOLD | COLOR_PAIR(1));
        mvprintw(2, (COLS - 25) / 2, " Layer: %s ", layerOptions[this->lastSelectionIndex].c_str());
        attroff(A_BOLD | COLOR_PAIR(1));

        int startY = (LINES - totalFields) / 2;
        int centerX = (COLS - 45) / 2;
        for(int i = 0; i < totalFields; ++i) {
            if (i == formIndex) {
                attron(COLOR_PAIR(2) | A_REVERSE);
                mvprintw(startY + i, centerX, " > %-20s [ %-20s ]", options[i].c_str(), this->concreteLayerInputs.at(this->lastSelectionIndex).at(i).c_str());
                attroff(COLOR_PAIR(2) | A_REVERSE);
            } else {
                mvprintw(startY + i, centerX, "   %-20s [ %-20s ]", options[i].c_str(),  this->concreteLayerInputs.at(this->lastSelectionIndex).at(i).c_str());
            }
        }
        int inputX = centerX + 26;
        this->showDoneOption(); 
        this->showBackOption(); 
        refresh();

        int ch = getch();
        this->moveIndex(formIndex, ch, totalFields);
        if (ch == 'b') {
            break; 
        }
        if(ch == ' ') {
            this->createFirewallRule();
            this->stop = true;
            break;
        }
        if (ch == '\n' || ch == 10) {
            move(startY + formIndex, inputX);
            echo();
            this->readInput();
            this->concreteLayerInputs.at(this->lastSelectionIndex).at(formIndex) =  std::string(this->currentUserInput);
            noecho();
        }
    }
}
void InteractiveCLIMode::showMainMenu() {
    int totalOptions = this->layerOptions.size();
    int startY = (LINES - totalOptions) / 2;

    while(!this->stop) {
        erase();
        this->showFrame(COLS, LINES); 
        for(int i = 0; i < totalOptions; ++i) {
            std::string text = this->layerOptions.at(i);
            int centerX = (COLS - text.length()) / 2;

            attron(A_BOLD);
            
            if (i == this->selectionIndex) attron(COLOR_PAIR(2)); 
            mvprintw(startY + i, centerX, "%s", text.c_str());

            if (i == this->selectionIndex) attroff(COLOR_PAIR(2));
            attroff(A_BOLD);
        }
        this->showBackOption();
        refresh();
        this->getSelectionIndex('0', '4');
        if(this->selectionIndex == 'b') {
            this->selectionIndex = 0;
            this->currentState = MenuState::MAIN;
            break;
        }
        if (this->selectionIndex == '\n') { 
            this->showConcreteLayerOption();
        }
    }

}


void InteractiveCLIMode::createSelectRule() {
    int ID = this->safeStoi(this->concreteSelectInputs.at(0),-1);
    bool fromMemory = this->isPermit(this->concreteSelectInputs.at(1));
    std::string& layer = this->concreteSelectInputs.at(2);
    bool permit = this->isPermit(this->concreteSelectInputs.at(3));

    std::shared_ptr<SelectRule> selectRule = std::make_shared<SelectRule>(permit,ID,layer,fromMemory);
    this->parsedFilterRule = std::make_shared<FilterRule>(selectRule,ID);
}

void InteractiveCLIMode::showSelectMenu() {
    int formIndex = 0; 
    auto& options = this->selectConcreteOptions;
    int totalFields = options.size();

    while(!this->stop) {
        erase();
        this->showFrame(COLS, LINES); 
        int startY = (LINES - totalFields) / 2;
        int centerX = (COLS - 45) / 2;
        for(int i = 0; i < totalFields; ++i) {
            if (i == formIndex) {
                attron(COLOR_PAIR(2) | A_REVERSE);
                mvprintw(startY + i, centerX, " > %-20s [ %-20s ]", options[i].c_str(), this->concreteSelectInputs.at(i).c_str());
                attroff(COLOR_PAIR(2) | A_REVERSE);
            } else {
                mvprintw(startY + i, centerX, "   %-20s [ %-20s ]", options[i].c_str(),  this->concreteSelectInputs.at(i).c_str());
            }
        }
        int inputX = centerX + 26;
        this->showDoneOption(); 
        this->showBackOption(); 
        refresh();
        int ch = getch();
        this->moveIndex(formIndex, ch, totalFields);
         if (ch == 'b') {
            break; 
        }
        if(ch == ' ') {
            this->stop = true;
            this->createSelectRule();
            break;
        }
        if (ch == '\n' || ch == 10) {
            move(startY + formIndex, inputX);
            echo();
            this->readInput();
            this->concreteSelectInputs.at(formIndex) =  std::string(this->currentUserInput);
            noecho();
        }
    }
}

void InteractiveCLIMode::showRedirectMenu() {
    int formIndex = 0; 
    auto& options = this->redirectConcreteOptions;
    int totalFields = options.size();

    while(!this->stop) {
        erase();
        this->showFrame(COLS, LINES); 
        int startY = (LINES - totalFields) / 2;
        int centerX = (COLS - 45) / 2;
        for(int i = 0; i < totalFields; ++i) {
            if (i == formIndex) {
                attron(COLOR_PAIR(2) | A_REVERSE);
                mvprintw(startY + i, centerX, " > %-20s [ %-20s ]", options[i].c_str(), this->concreteRedirectInputs.at(i).c_str());
                attroff(COLOR_PAIR(2) | A_REVERSE);
            } else {
                mvprintw(startY + i, centerX, "   %-20s [ %-20s ]", options[i].c_str(),  this->concreteRedirectInputs.at(i).c_str());
            }
        }
        int inputX = centerX + 26;
        this->showDoneOption(); 
        this->showBackOption(); 
        refresh();
        int ch = getch();
        this->moveIndex(formIndex, ch, totalFields);
         if (ch == 'b') {
            break; 
        }
        if(ch == ' ') {
            this->stop = true;
            this->createRedirectRule();
            break;
        }
        if (ch == '\n' || ch == 10) {
            move(startY + formIndex, inputX);
            echo();
            this->readInput();
            this->redirectConcreteOptions.at(formIndex) =  std::string(this->currentUserInput);
            noecho();
        }
    }
}

void InteractiveCLIMode::showRemoveMenu() {
    int formIndex = 0; 
    auto& options = this->removeConcreteOptions;
    int totalFields = options.size();

    while(!this->stop) {
        erase();
        this->showFrame(COLS, LINES); 
        int startY = (LINES - totalFields) / 2;
        int centerX = (COLS - 45) / 2;
        for(int i = 0; i < totalFields; ++i) {
            if (i == formIndex) {
                attron(COLOR_PAIR(2) | A_REVERSE);
                mvprintw(startY + i, centerX, " > %-20s [ %-20s ]", options[i].c_str(), this->concreteRemoveInputs.at(i).c_str());
                attroff(COLOR_PAIR(2) | A_REVERSE);
            } else {
                mvprintw(startY + i, centerX, "   %-20s [ %-20s ]", options[i].c_str(),  this->concreteRemoveInputs.at(i).c_str());
            }
        }
        int inputX = centerX + 26;
        this->showDoneOption(); 
        this->showBackOption(); 
        refresh();
        int ch = getch();
        this->moveIndex(formIndex, ch, totalFields);
         if (ch == 'b') {
            break; 
        }
        if(ch == ' ') {
            this->stop = true;
            this->createRemoveRule();
            break;
        }
        if (ch == '\n' || ch == 10) {
            move(startY + formIndex, inputX);
            echo();
            this->readInput();
            this->concreteRemoveInputs.at(formIndex) =  std::string(this->currentUserInput);
            noecho();
        }
    }
}


void InteractiveCLIMode::createRemoveRule() {
    int ID = this->safeStoi(this->concreteRemoveInputs.at(0),-1);
    bool fromMemory = this->isPermit(this->concreteRemoveInputs.at(1));
    std::string& layer = this->concreteRemoveInputs.at(2);

    std::shared_ptr<RemoveRule> removeRule = std::make_shared<RemoveRule>(ID,fromMemory,layer);
    this->parsedFilterRule = std::make_shared<FilterRule>(removeRule,ID);
}


void InteractiveCLIMode::createRedirectRule() {
    bool permit = this->isPermit(this->concreteRedirectInputs.at(0));
    std::string& layer = this->concreteRedirectInputs.at(1);
    int count = this->safeStoi(this->concreteRedirectInputs.at(2),0);

    std::shared_ptr<RedirectRule> redirectRule = std::make_shared<RedirectRule>(permit,layer,count);

    this->parsedFilterRule = std::make_shared<FilterRule>(redirectRule,-1);

}

void InteractiveCLIMode::showMenuLayout() {
    this->ruleIndex = this->lastSelectionIndex;
    switch (static_cast<MenuState>(this->lastSelectionIndex)) {
        case InteractiveCLIMode::MenuState::REDIRECT:
            this->showRedirectMenu();
            break;
        case InteractiveCLIMode::MenuState::REMOVE:
            this->showRemoveMenu();
            break;
        case InteractiveCLIMode::MenuState::SELECT:
            this->showSelectMenu();
            break;
        case InteractiveCLIMode::MenuState::MAIN:
        default:
            this->showMainMenu();
    }
}

void InteractiveCLIMode::showLayerOptions() {
    int totalOptions = this->layerOptions.size();
    int startY = (LINES - totalOptions) / 2;


    for(int i = 0; i < totalOptions; ++i) {
        std::string text = this->layerOptions.at(i);
        int centerX = (COLS - text.length()) / 2;

        attron(A_BOLD);
        
        if (i == 0) attron(COLOR_PAIR(2)); 
        mvprintw(startY + i, centerX, "%s", text.c_str());

        if (i == 0) attroff(COLOR_PAIR(2));
        attroff(A_BOLD);
    }
}
void InteractiveCLIMode::readInput() {
    std::memset(this->currentUserInput,0,USER_INPUT_SIZE);
    getnstr(this->currentUserInput, USER_INPUT_SIZE);
}



void InteractiveCLIMode::showFrame(int w,int h) {
    attron(COLOR_PAIR(5));
    for(int i = 0; i < h; ++i) {
        for(int j = 0; j < w; ++j) {
            if(i == 0 || i == h -1) {
                mvaddch(i, j, ' ');     
            } else {
                if(j == 0 || j == w - 1) {
                    mvaddch(i, j, ' ');     
                }
            }   
        }
    }
    attroff(COLOR_PAIR(1));
}


void InteractiveCLIMode::showLayout() {
    this->showFrame(COLS, LINES);

    const char* art[] = {
    "+------------------------------------------------------------+",
    "|  [ PRESS 1-4 ]  T O   S T A R T   S E L E C T I O N        |",
    "+------------------------------------------------------------+"
    };
    int artRows = 3;
    int artCols = 64; 
    int startY = (LINES / 4) - 1; 
    int startX = (COLS - artCols) / 2;

    attron(COLOR_PAIR(1) | A_BOLD); 
    for (int i = 0; i < artRows; i++) {
        mvprintw(startY + i, startX, "%s", art[i]);
    }
    attroff(COLOR_PAIR(1) | A_BOLD);

    for(int i = 0; i < this->selectionOptions.size(); ++i) {
            std::string text = this->selectionOptions.at(i);
            int centerX = (COLS - text.length()) / 2;

            attron(A_BOLD);
            
            if (i == this->selectionIndex) attron(COLOR_PAIR(2)); 
            mvprintw(startY + i + 4, centerX, "%s", text.c_str());

            if (i == this->selectionIndex) attroff(COLOR_PAIR(2));
            attroff(A_BOLD);
    }
    this->showQuitOption();
}