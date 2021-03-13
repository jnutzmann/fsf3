
#include "cli.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

namespace shell {


class HelpCommand : public Command {
  public:
    HelpCommand() :
      Command("help", "displays available commands") {}

    void SetCommandListHead(CommandListEntry* commandListHead) {
      _commandListHead = commandListHead;
    }

    void Callback(char* commandInput) {

      CommandListEntry* e = _commandListHead;
      while (e) {
        printf("%s: %s\r\n", e->command->command, e->command->helpString);
        e = e->next;
      }
    }

  private:
    CommandListEntry* _commandListHead;
};

static HelpCommand helpCommand;

Command::Command(const char* command, const char* helpString) :
    command(command),
    helpString(helpString) { }

void Command::Callback(char* commandInput) {
  printf("Not Implemented");
}

CommandInterperter::CommandInterperter() {

  _initialEntry.command = &helpCommand;
  _initialEntry.next = NULL;

  _commandListHead = &_initialEntry;
  _commandListTail = &_initialEntry;

  helpCommand.SetCommandListHead(_commandListHead);
}

void CommandInterperter::Register(Command *command) {
  CommandListEntry* newCommand = (CommandListEntry*) malloc(sizeof(CommandListEntry));
  newCommand->command = command;
  newCommand->next = NULL;

  _commandListTail->next = newCommand;
  _commandListTail = newCommand;
}

void CommandInterperter::Interpert(char* commandInput) {
  CommandListEntry* e = _commandListHead;
  Command* cmd = NULL;
  while (e) {
    int cmdLen = strlen(e->command->command);
    if (strncmp(commandInput, e->command->command, cmdLen) == 0) {
      if (commandInput[cmdLen] == 0 || commandInput[cmdLen] == ' ') {
        cmd = e->command;
        break;
      }
    }
    e = e->next;
  }

  if (cmd) {
    cmd->Callback(commandInput);
  } else {
    printf("Command '%s' not found.  Try 'help' command to see what's available.", commandInput);
  }
}


//CommandInterperter commandInterperterInstance((Command*) &helpCommand);


} // namespace shell
