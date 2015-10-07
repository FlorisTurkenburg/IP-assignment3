#include "paperserver.h"

// The head of the linked list of papers.
struct articles *first_paper = NULL; 


article_num *add_1_svc(add_article_in *in, struct svc_req *rqstp) {
    static article_num out;
    struct articles *cur_paper;
    struct articles *new_paper;
    struct articles *free_number_predecessor;
    new_paper = (struct articles *) malloc(sizeof(struct articles));
    
    new_paper->author = strdup(in->author);
    new_paper->title = strdup(in->title);
    new_paper->content.content_len = in->content.content_len;
    new_paper->content.content_val = malloc(in->content.content_len * sizeof(char));
    memcpy(new_paper->content.content_val, in->content.content_val, in->content.content_len * sizeof(char));

    if (first_paper == NULL) { // Add the very first paper to the list
        new_paper->num = 1;
        out = 1;
        // printf("in_author%s\tin_title%s\n", in->author, in->title);
        // printf("paper_author%s\tpaper_title%s\n", paper->author, paper->title);
        new_paper->next = NULL;
        first_paper = new_paper;
        // free(new_paper);
        // printf("firstpaper_author %s\tfirstpaper_title %s\n", first_paper->author, first_paper->title);
    }
    else {
        article_num first_free = 0;
        int look_for_free = 1;
        cur_paper = first_paper;

        if (first_paper->num > 1) {
            first_free = 1;
            look_for_free = 0;
        }
        while (cur_paper) {

            // printf("in_author %s\tin_title %s\n", in->author, in->title);
            // printf("curpaper_author %s\tcurpaper_title %s\n", cur_paper->author, cur_paper->title);
            if (strcmp(cur_paper->author, strdup(in->author)) == 0 && strcmp(cur_paper->title, strdup(in->title)) == 0) {
                // Paper already exists, update and return number
                cur_paper->content.content_len = in->content.content_len;
                cur_paper->content.content_val = realloc(cur_paper->content.content_val, in->content.content_len * sizeof(char));
                memcpy(cur_paper->content.content_val, in->content.content_val, in->content.content_len * sizeof(char)); 
                out = cur_paper->num;
                free(new_paper);
                return &out;
            }
            else { // Continue looking if paper exists
                if (cur_paper->next) {
                    if (look_for_free == 1) {
                        // Check if there is a free number between the current 
                        // and next paper in the list.
                        if ((cur_paper->next->num - cur_paper->num) > 1) {
                            first_free = cur_paper->num + 1;
                            // Store the pointer to the paper preceding the free 
                            // number, this prevents having to iterate through
                            // the linked list again later to find it.
                            free_number_predecessor = cur_paper;
                            look_for_free = 0;
                        }
                    }
                    cur_paper = cur_paper->next;
                }
                else { // End of the list reached
                    break;
                }
            }
        }
        if (look_for_free == 1) { 
            // No free number was found so add the paper at the end
            new_paper->num = cur_paper->num + 1;
            out = new_paper->num;
            new_paper->next = NULL;
            cur_paper->next = new_paper;
        }
        else { // Place the new paper at the free number
            if (first_free == 1) { // If number 1 is available, place the paper
                                   // at the start of the list.
                new_paper->num = 1;
                new_paper->next = first_paper;
                first_paper = new_paper;
                out = 1;

            }
            else {
                new_paper->num = first_free;
                new_paper->next = free_number_predecessor->next;
                free_number_predecessor->next = new_paper;
                out = first_free;

                // cur_paper = first_paper;
                // while (cur_paper) {
                //     if (cur_paper->num == (first_free - 1)) {
                //         new_paper->num = first_free;
                //         new_paper->next = cur_paper->next;
                //         cur_paper->next = new_paper;
                //         out = first_free;
                //         break;
                //     }
                // }
            }
            
        }


    }
    

    return &out;
}


list_article_out *list_1_svc(void * not_used, struct svc_req *rqstp) {
    static struct list_article_out *article_list_start;
    struct list_article_out *cur_out_article;
    struct articles *paper = first_paper;

    if (paper == NULL) {
        article_list_start = NULL;
        return article_list_start;
    }

    article_list_start = (struct list_article_out*) malloc(sizeof(struct list_article_out));
    cur_out_article = article_list_start;

    while (paper) {
        cur_out_article->num = paper->num;
        cur_out_article->author = strdup(paper->author);
        cur_out_article->title = strdup(paper->title);

        if (paper->next) {
            cur_out_article->next = (struct list_article_out *) malloc(sizeof(struct list_article_out));
            cur_out_article = cur_out_article->next;
            paper = paper->next;
        }
        else {
            cur_out_article->next = NULL;
            break;
        }
    }

    return article_list_start;
        
}   


article_info_out *info_1_svc(article_num *num, struct svc_req *reqstp) {
    static article_info_out *article;
    struct articles *paper;
    //paper = (struct articles *) malloc(sizeof(struct articles));
    article = (struct article_info_out *) malloc(sizeof(struct article_info_out));

    // printf("firstpaper_author %s\tfirstpaper_title %s\n", first_paper->author, first_paper->title);

    paper = first_paper;

    while (paper != NULL) {
        if (paper->num == *num) {
            article->author = strdup(paper->author);
            article->title = strdup(paper->title);
            break;
        }
        else {
            if (paper->next) {
                paper = paper->next;
            }
            else {
                article = NULL;
                break;
            }
        }
    }

    return article;   
}   

int *remove_1_svc(article_num *num, struct svc_req *rqstp) {
    static int ret_val;
    
    struct articles *cur_paper;
    struct articles *temp_paper;

    ret_val = 0;
    if (first_paper) {
        cur_paper = first_paper;
        if (first_paper->num == *num) { 
            // In case the first paper in the list must be removed, assign the next
            // as the start of the list and remove the paper. 
            printf("remove first paper\n");
                                    
            first_paper = first_paper->next;
            free(cur_paper);
            ret_val = 1;
            return &ret_val;
        }
        // Search for the correct paper, if found, link its predecessor and 
        // successor together and remove the paper.
        while (cur_paper) {
            temp_paper = cur_paper->next;
            if (temp_paper) {
                printf("temp_paper num: %ld \n", temp_paper->num);
                if (temp_paper->num == *num) {
                    cur_paper->next = temp_paper->next;
                    free(temp_paper->content.content_val);
                    free(temp_paper);
                    ret_val = 1;
                    break;
                }
                else {
                    cur_paper = cur_paper->next;
                }
                
            }
            else {
                break;
            }
        }
        
    }

    // Return value is not used.
    return &ret_val;

}

fetch_article_out *fetch_1_svc(article_num *num, struct svc_req *rqstp) {
    static struct fetch_article_out *article;
    struct articles *paper = first_paper;

    article = (struct fetch_article_out *) malloc(sizeof(struct fetch_article_out));


    while (paper != NULL) {
        if (paper->num == *num) {
            article->content.content_len = paper->content.content_len;
            article->content.content_val = malloc(paper->content.content_len * sizeof(char));
            memcpy(article->content.content_val, paper->content.content_val, paper->content.content_len * sizeof(char));
            
            return article;
        }
        else {
            if (paper->next != NULL) {
                paper = paper->next;
            }
            else {
                break;
            }
        }
    }
    article->content.content_len = 0;
    article->content.content_val = NULL;

    return article;


}