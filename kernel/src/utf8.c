#include <kernel/utf8.h>

struct head {
        u8 bit_7 : 1;
        u8 bit_6 : 1;
        u8 bit_5 : 1;
        u8 bit_4 : 1;
        u8 bit_3 : 1;
        u8 bit_2 : 1;
        u8 bit_1 : 1;
        u8 bit_0 : 1;
} __attribute__((packed));

u8 utf8_width(const char *c)
{
        const struct head *head = (struct head *)c;
        if(head->bit_0 == 0)
                return 1;
        else {
                if(head->bit_1 == 1
                        && head->bit_2 == 0) return 2;
                else if(head->bit_1 == 1
                        && head->bit_2 == 1
                        && head->bit_3 == 0) return 3;
                else if(head->bit_1 == 1
                        && head->bit_2 == 1
                        && head->bit_3 == 1
                        && head->bit_4 == 0) return 4;
        }

        return 1;
}

u32 utf8_code(const u8 *c_ptr, u8 width)
{
        const u8 * volatile c         = c_ptr;
        volatile u32 code_point = 0;
        volatile u32 val        = (u32)(*c);

        switch (width) {
                case 1:
                        val = val & 0x7F;
                        code_point |= val;
                        break;
                case 2:
                        val = (val & 0x1F) << 6;
                        code_point |= val;

                        c++;
                        val = (u32)(*c);
                        val = val & 0x3F;
                        code_point |= val;
                        break;
                case 3:
                        val = (val & 0xF) << 12;
                        code_point |= val;

                        c++;
                        val = (u32)(*c);
                        val = (val & 0x3F) << 6;
                        code_point |= val;

                        c++;
                        val = (u32)(*c);
                        val = val & 0x3F;
                        code_point |= val;
                        break;
                case 4:
                        val = (val & 0x7) << 18;
                        code_point |= val;

                        c++;
                        val = (u32)(*c);
                        val = (val & 0x3F) << 12;
                        code_point |= val;

                        c++;
                        val = (u32)(*c);
                        val = (val & 0x3F) << 6;
                        code_point |= val;

                        c++;
                        val = (u32)(*c);
                        val = (val & 0x3F) << 6;
                        code_point |= val;
                        break;
        }

        return code_point;
}
