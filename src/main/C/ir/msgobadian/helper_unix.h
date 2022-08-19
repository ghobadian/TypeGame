#pragma once
#include <termios.h>
#include <pthread.h>
#include <stdio.h>

void *keypress_callback(void *args)
{
  void (*callback)(char) = (void (*)(char))(args);

  while (1)
    callback(getchar());

  return 0;
}

void config_console()
{
  struct termios terminal_info;
  tcgetattr(0, &terminal_info);
  terminal_info.c_lflag &= ~(ICANON | ECHO);
  terminal_info.c_cc[VTIME] = 0;
  terminal_info.c_cc[VMIN] = 1;
  tcsetattr(0, TCSANOW, &terminal_info);
}

pthread_t start_listening(void (*callback)(char))
{
  config_console();
  pthread_t thread_id;
  pthread_create(&thread_id, NULL, &keypress_callback, (void *)(callback));

  return thread_id;
}