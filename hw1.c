#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/moduleparam.h>
#include <linux/sysfs.h>
#include <linux/string.h>

MODULE_LICENSE("Dual BSD/GPL");

static struct kobject *hw1_kobject;

static int mask = 111;
static char *name1 = "swap_string";
static char *name2 = "calc";
static char *name3 = "sum_tree";

module_param(mask, int, S_IRUGO);
module_param(name1, charp, S_IRUGO);
module_param(name2, charp, S_IRUGO);
module_param(name3, charp, S_IRUGO);

static char r1[100];
static char r2[100];
static char r3[100];

static ssize_t q1_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    int swap = 0, cnt = 0, i = 0;
    char tmp[100], result[100];
    while(r1[i]!=' ')
    {
        swap *= 10;
        swap += r1[i]-'0';
        i++;
    }
    i++;
    while(r1[i]!='\0')
    {
        tmp[cnt] = r1[i];
        cnt++;
        i++;
    }
    cnt--;
    memset(result, '\0', sizeof(result));
    for(i=0; i<swap; i++)
        result[i-swap+cnt] = tmp[i];
    for(; i<cnt; i++)
        result[i-swap] = tmp[i];
    return sprintf(buf, "%s\n", result);
}
static ssize_t q1_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    memset(r1, '\0', sizeof(r1));
    memcpy(r1, buf, strlen(buf));
    return count;
}
static ssize_t q2_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    int cnt = 0, save = 0, i = 0, opei = 0, inti = 0, result;
    int intStack[50];
    char highOpe = '\0';
    char lowOpe[50];
    while(r2[cnt]!='\0')
        cnt++;
    cnt--;
    for(i=0; i<50; i++)
        intStack[i] = 1;
    memset(lowOpe, '\0', sizeof(lowOpe));
    for(i=0; i<cnt; i++)
    {
        if(r2[i]>='0' && r2[i]<='9')
        {
            save *= 10;
            save += r2[i]-'0';
        }
        else if(r2[i]=='+' || r2[i]=='-')
        {
            if(r2[i]!='-' || r2[i+1]==' ')
            {
                if(highOpe=='*')
                intStack[inti-1] *= save;
                else if(highOpe=='/')
                   intStack[inti-1] /= save;
                else if(highOpe=='%')
                    intStack[inti-1] %= save;
                else
                    intStack[inti++] *= save;
                lowOpe[opei++] = r2[i];
                highOpe = '\0';
                save = 0;
            }
            else
            {
                if(highOpe=='*' || highOpe=='/')
                    intStack[inti-1] *= -1;
                else if(highOpe!='%')
                    intStack[inti] *= -1;
            }
        }
        else if(r2[i]=='*' || r2[i]=='/' || r2[i]=='%')
        {
            if(highOpe=='*')
                intStack[inti-1] *= save;
            else if(highOpe=='/')
                intStack[inti-1] /= save;
            else if(highOpe=='%')
                intStack[inti-1] %= save;
            else
                intStack[inti++] *= save;
            highOpe = r2[i];
            save = 0;
        }
    }
    if(highOpe=='*')
        intStack[inti-1] *= save;
    else if(highOpe=='/')
        intStack[inti-1] /= save;
    else if(highOpe=='%')
        intStack[inti-1] %= save;
    else
        intStack[inti++] *= save;

    result = intStack[0];
    for(i=0; i<opei; i++)
    {
        if(lowOpe[i]=='+')
            result += intStack[i+1];
        else if(lowOpe[i]=='-')
            result -= intStack[i+1];
    }
    return sprintf(buf, "%d\n", result);
}
static ssize_t q2_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    memset(r2, '\0', sizeof(r2));
    memcpy(r2, buf, strlen(buf));
    return count;
}
static ssize_t q3_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    int cnt = 0, leaf = 0, level = 0, total = 0, i = 0, j = 0;
    int tree[50], tmp[50];
    char result[100];
    while(r3[cnt]!='\0')
        cnt++;
    cnt--;
    memset(tmp, 0, sizeof(tmp));
    memset(result, '\0', sizeof(result));
    for(; i<cnt; i++)
    {
        if(r3[i]>='0' && r3[i]<='9')
        {
            leaf *= 10;
            leaf += r3[i]-'0';
        }
        else if(r3[i]==' ')
        {
            if(r3[i-1]>='0' && r3[i-1]<='9')
            {
                for(j=0; j<level; j++)
                    tmp[total] += tree[j];
                tmp[total++] += leaf;
            }
            leaf = 0;
        }
        else if(r3[i]=='(')
        {
            tree[level] = leaf;
            level++;
            leaf = 0;
        }
        else if(r3[i]==')')
        {
            if(r3[i-1]>='0' && r3[i-1]<='9')
            {
                for(j=0; j<level; j++)
                    tmp[total] += tree[j];
                tmp[total++] += leaf;
            }
            level--;
            leaf = 0;
        }
    }
    cnt = 0;
    cnt += sprintf(result+cnt, "%d", tmp[0]);
    for(i=1; i<total; i++)
    {
        cnt += sprintf(result+cnt, ", %d", tmp[i]);
    }
    return sprintf(buf, "%s\n", result);
}
static ssize_t q3_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    memset(r3, '\0', sizeof(r3));
    memcpy(r3, buf, strlen(buf));
    return count;
}

struct kobj_attribute q1_attribute = __ATTR(name1, 0660, q1_show, q1_store);
struct kobj_attribute q2_attribute = __ATTR(name2, 0660, q2_show, q2_store);
struct kobj_attribute q3_attribute = __ATTR(name3, 0660, q3_show, q3_store);

static int __init create_hello(void)
{
    int error;
    printk(KERN_ALERT "Hello!\n");
    hw1_kobject = kobject_create_and_add("hw1", kernel_kobj);
    if(mask>=100)
    {
        if((mask/100)%10==1)
        {
            q1_attribute.attr.name = name1;
            error = sysfs_create_file(hw1_kobject, &q1_attribute.attr);
        }
        if((mask/10)%10==1)
        {
            q2_attribute.attr.name = name2;
            error = sysfs_create_file(hw1_kobject, &q2_attribute.attr);
        }
        if(mask%10==1)
        {
            q3_attribute.attr.name = name3;
            error = sysfs_create_file(hw1_kobject, &q3_attribute.attr);
        }
    }
    else
    {
        if((mask/8)%8==1)
        {
            q2_attribute.attr.name = name2;
            error = sysfs_create_file(hw1_kobject, &q2_attribute.attr);
        }
        if(mask%8==1)
        {
            q3_attribute.attr.name = name3;
            error = sysfs_create_file(hw1_kobject, &q3_attribute.attr);
        }
    }
    return 0;
}
static void __exit cleanup_hello(void)
{
    printk(KERN_ALERT "Goodbye!\n");
    kobject_put(hw1_kobject);
}

module_init(create_hello);
module_exit(cleanup_hello);
