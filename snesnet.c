#include "snesnet.h"
#include "mongoose.h"
#include "glob.h"
#include "stdlib.h"
#include "mcp23017.h"

#define CONTROLLER_ONE_LATCH 20
#define CONTROLLER_TWO_LATCH 21

#define size 12

typedef struct
{
  int active;
  int pinNum;
} ButtonToggle;

ButtonToggle controllerOneButtons[size] = {
    {1, 4},
    {1, 18},
    {1, 27},
    {1, 22},
    {1, 23},
    {1, 24},
    {1, 25},
    {1, 5},
    {1, 6},
    {1, 12},
    {1, 13},
    {1, 16}};

// Prefixing mcp23017 buttons with 100
ButtonToggle controllerTwoButtons[size] = {
    {1, 100},
    {1, 101},
    {1, 102},
    {1, 103},
    {1, 104},
    {1, 105},
    {1, 106},
    {1, 107},
    {1, 108},
    {1, 109},
    {1, 110},
    {1, 111}};

static void
callback(struct mg_connection *connection, int event, void *event_data, void *callback_data)
{
  if (event == MG_EV_HTTP_MSG)
  {
    struct mg_http_message *message = (struct mg_http_message *)event_data;
    struct mg_str caps[2];

    if (mg_match(message->uri, mg_str("/api/down/*"), caps))
    {
      int pinDown;
      sscanf(caps[0].ptr, "%d", &pinDown);
      int i;
      for (i = 0; i < size; ++i)
      {
        if (controllerOneButtons[i].pinNum == pinDown)
        {
          controllerOneButtons[i].active = 0;
        }

        if (controllerTwoButtons[i].pinNum == pinDown)
        {
          controllerTwoButtons[i].active = 0;
        }
      }

      mg_http_reply(connection, 200, "Content-Type: text/plain\r\n", "%s", "OK");
    }
    else if (mg_match(message->uri, mg_str("/api/up/*"), caps))
    {
      int pinUp;
      sscanf(caps[0].ptr, "%d", &pinUp);
      int i;
      for (i = 0; i < size; ++i)
      {
        if (controllerOneButtons[i].pinNum == pinUp)
        {
          controllerOneButtons[i].active = 1;
        }

        if (controllerTwoButtons[i].pinNum == pinUp)
        {
          controllerTwoButtons[i].active = 1;
        }
      }
      mg_http_reply(connection, 200, "Content-Type: text/plain\r\n", "%s", "OK");
    }
    else
    {
      mg_http_reply(connection, 404, "", "%s", "NOT FOUND");
    }
  }
}

void setup(void)
{
  int i;
  for (i = 0; i < size; ++i)
  {
    pinMode(controllerOneButtons[i].pinNum, OUTPUT);
    pinMode(controllerTwoButtons[i].pinNum, OUTPUT);
  }
}

void clear_all_buttons(void)
{
  int i;
  for (i = 0; i < size; ++i)
  {
    digitalWrite(controllerOneButtons[i].pinNum, HIGH);
    digitalWrite(controllerTwoButtons[i].pinNum, HIGH);
  }
}

/**
 * Pulses every 16.67ms
 * Pulse length is 12µs
 */
void latch_lock(void)
{
  int i;
  for (i = 0; i < size; ++i)
  {
    digitalWrite(controllerOneButtons[i].pinNum, controllerOneButtons[i].active);
  }
}

/**
 * Pulses every 16.67ms
 * Pulse length is 12µs
 */
void latch_lock_two(void)
{
  int i;
  for (i = 0; i < size; ++i)
  {
    digitalWrite(controllerTwoButtons[i].pinNum, controllerTwoButtons[i].active);
  }
}

int main(void)
{
  if (wiringPiSetupGpio() == -1)
  {
    fprintf(stdout, "oops: %s\n", strerror(errno));
    return 1;
  }
  int i;

  mcp23017Setup(100, 0x20);
  fprintf(stdout, "Setting up GPIO\n");

  pinMode(CONTROLLER_ONE_LATCH, INPUT);
  pinMode(CONTROLLER_TWO_LATCH, INPUT);

  pullUpDnControl(CONTROLLER_ONE_LATCH, PUD_DOWN);
  pullUpDnControl(CONTROLLER_TWO_LATCH, PUD_DOWN);

  setup();
  clear_all_buttons();

  wiringPiISR(CONTROLLER_ONE_LATCH, INT_EDGE_FALLING, &latch_lock);
  fprintf(stdout, "LATCH ENABLED\n");

  wiringPiISR(CONTROLLER_TWO_LATCH, INT_EDGE_FALLING, &latch_lock_two);
  fprintf(stdout, "LATCH ENABLED 2\n");

  struct mg_mgr mgr;
  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, "0.0.0.0:8000", callback, NULL); // Create listening connection
  for (;;)
    mg_mgr_poll(&mgr, 1000); // Block forever
  mg_mgr_free(&mgr);
  return 0;
}
