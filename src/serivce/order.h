#ifndef ORDER_H_
#define ORDER_H_

enum order_type {
    ORDER_TYPE_STD,
    ORDER_TYPE_EXPRESS,
    ORDER_TYPE_SELFPICK
};

enum order_status {
    ORDER_STATUS_PACKING,
    ORDER_STATUS_ON_THE_WAY,
    ORDER_STATUS_PICKED
};

struct order;

#endif /* ORDER_H_ */
