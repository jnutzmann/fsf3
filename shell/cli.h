#ifndef SHELL_CLI_H_
#define SHELL_CLI_H_

#include <stdint.h>

namespace shell {

class Command {
  public:
    Command(const char* command, const char* helpString);

    const char* command;
    const char* helpString;

    virtual void Callback(char* commandInput);
};

typedef struct CommandListEntry {
    Command* command;
    struct CommandListEntry* next;
} CommandListEntry;


class CommandInterperter {
  public:

    CommandInterperter();

    void Register(Command *command);
    void Interpert(char* commandInput);

  private:

    CommandListEntry* _commandListHead;
    CommandListEntry* _commandListTail;

    CommandListEntry _initialEntry;
};

extern CommandInterperter commandInterperterInstance;

} // namespace shell



#endif /* SHELL_CLI_H_ */
