#include "snesnet.h"
#include "mongoose.h"
#include "glob.h"
#include "stdlib.h"

#define LATCH_IN 15
#define size 12

typedef struct
{
  int active;
  int pinNum;
} ButtonToggle;

ButtonToggle buttons[size] = {
    {1, 17},
    {1, 27},
    {1, 22},
    {1, 5},
    {1, 6},
    {1, 19},
    {1, 26},
    {1, 21},
    {1, 20},
    {1, 16},
    {1, 25},
    {1, 24}};

static void callback(struct mg_connection *connection, int event, void *event_data, void *callback_data)
{
  if (event == MG_EV_HTTP_MSG)
  {
    struct mg_http_message *message = (struct mg_http_message *) event_data;
    struct mg_str caps[2];

    if (mg_match(message->uri, mg_str("/api/down/*"), caps))
    {
      int pinDown;
      sscanf(caps[0].ptr, "%d", &pinDown);
      int i;
      for (i = 0; i < size; ++i)
      {
        if (buttons[i].pinNum == pinDown)
        {
          buttons[i].active = 0;
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
        if (buttons[i].pinNum == pinUp)
        {
          buttons[i].active = 1;
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
    pinMode(buttons[i].pinNum, OUTPUT);
  }
}

void clear_all_buttons(void)
{
  int i;
  for (i = 0; i < size; ++i)
  {
    digitalWrite(buttons[i].pinNum, HIGH);
  }
}

void latch_lock(void)
{
  int i;
  for (i = 0; i < size; ++i)
  {
    digitalWrite(buttons[i].pinNum, buttons[i].active);
  }
}

int main(void)
{
  if (wiringPiSetupGpio() == -1)
  {
    fprintf(stdout, "oops: %s\n", strerror(errno));
    return 1;
  }

  fprintf(stdout, "Setting up GPIO\n");
  pinMode(LATCH_IN, INPUT);
  pullUpDnControl(LATCH_IN, PUD_DOWN);
  setup();
  clear_all_buttons();
  wiringPiISR(LATCH_IN, INT_EDGE_FALLING, &latch_lock);
  fprintf(stdout, "LATCH ENABLED\n");

  struct mg_mgr mgr;
  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, "0.0.0.0:8000", callback, NULL); // Create listening connection
  for (;;)
    mg_mgr_poll(&mgr, 200); // Block forever
  mg_mgr_free(&mgr);
  return 0;
}
