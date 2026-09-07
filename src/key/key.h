#ifndef __KEY_H
#define __KEY_H

enum key_id_e{
    KEY_0_em = 0,
    KEY_1_em,
    KEY_2_em,
    KEY_3_em,
    KEY_NUM_em,
};

enum key_event_e {
    KEY_EVENT_NONE_em = 0,
    KEY_EVENT_SINGLE_em,
    KEY_EVENT_DOUBLE_em,
    KEY_EVENT_LONG_em,
    KEY_EVENT_REPEAT_em,
};

typedef  void (* key_event_callback_t)(enum key_id_e key_id_em,enum key_event_e event);


//外部其实

/* key_event.h */
struct key_subscriber_t {
    struct key_subscriber_t *next_pst;
    key_event_callback_t callback_pst;   /* 改成 _pst 后缀 */
    const char *name_pst;                 /* 改成 _pst 后缀 */
};

void key_event_subscribe(struct key_subscriber_t* sub_pst);

//自动注册函数 _name 和 _sub_init 前面绑定 _callback _user_name 分别对应结构体的值
#define KEY_SUBSCRIBE_DEFINE(_name, _callback, _user_name)      \
    static struct key_subscriber_t _name##_sub_st = {           \
        .callback_pst = _callback,                              \
        .name_pst = _user_name,                                 \
    };                                                          \
    static int _name##_sub_init(void)                           \
    {                                                           \
        key_event_subscribe(&_name##_sub_st);                   \
        return 0;                                               \
    }                                                           \
    SYS_INIT(_name##_sub_init, APPLICATION, 50)
#endif