#include "charter.h"
#include "tinyexpr/tinyexpr.h"

#include <stdlib.h>
#include <stdio.h>

chart * 
initialize_empty_chart()
{
    chart * new_chart = malloc(sizeof(chart));
    new_chart->width = 600;
    new_chart->height = 400;
    new_chart->title = NULL;

    new_chart->n_plots = 0;

    new_chart->x_axis.autoscale = TRUE;
    new_chart->x_axis.mode = LINEAR;
    new_chart->x_axis.label = NULL;

    new_chart->y_axis.autoscale = TRUE;
    new_chart->y_axis.mode = LINEAR;
    new_chart->y_axis.label = NULL;

    new_chart->plots = clist_new(NULL);

    return new_chart; 
}


double plot_get_max_x(plot* p)
{
    if (!p || !p->n)
        return 0;
    if (p->x_data == NULL)
        return p->n;
    double max_x = p->x_data[0];
    unsigned int i;
    for (i = 1; i<p->n;i++)
    {
        if (p->x_data[i] > max_x)
            max_x = p->x_data[i];
    }
    return max_x;
}

double plot_get_min_x(plot* p)
{
    if (!p || !p->n)
        return 0;
    if (p->x_data == NULL)
        return 1;
    double min_x = p->x_data[0];
    unsigned int i;
    for (i = 1; i<p->n;i++)
    {
        if (p->x_data[i] < min_x)
            min_x = p->x_data[i];
    }
    return min_x;
}

double * eval_math(plot *p)
{
    if (p->y_type != MATH || p->y_data == NULL || p->n == 0)
        return NULL;
    double * eval = malloc(p->n*sizeof(double));
    double x;
    te_variable vars[] = {{"x", &x}};

    int err;
    /* Compile the expression with variables. */
    te_expr *expr = te_compile((const char*)p->y_data, vars, 1, &err);
    if (expr) {
        unsigned int i;
        for (i = 0; i < p->n ;i++)
        {
            x = p->x_data[i];            
            eval[i] = te_eval(expr);
        }
    }
    return eval;
}

double * plot_eval_y(plot *p)
{
    if (p->y_type == MATH){
        return eval_math(p); 
    }
    if (p->n && p->y_data != NULL)
    {
        double * data = malloc(p->n * sizeof(double));
        unsigned int i = 0;
        for (i = 0; i < p->n; i++)
        {
            data[i] = ((double*)p->y_data)[i];
        }
        return data;
    }
    
    return NULL;
}


double plot_get_max_y(plot* p)
{
    if (!p || !p->n || !p->y_data)
        return 0;

    double * data = plot_eval_y(p);
   
    double max_x = data[0];
    unsigned int i;
    for (i = 1; i<p->n;i++)
    {
        if (data[i] > max_x)
            max_x = data[i];
    }
    free(data);
    return max_x;
}

double plot_get_min_y(plot* p)
{
    if (!p || !p->n || !p->y_data)
        return 0;
    double * data = plot_eval_y(p);

    double min_x = data[0];
    unsigned int i;
    for (i = 1; i<p->n;i++)
    {
        if (data[i] < min_x)
            min_x = data[i];
    }
    if (p->type == BAR && min_x > 0)
    {
        return 0;
    }
    free(data);
    return min_x;
}


double chart_get_max_x(chart *c)
{
    if (c->x_axis.autoscale == TRUE){
        if (!c->n_plots)
            return 0;
        unsigned int i;
        double M = plot_get_max_x(c->plots->data);
        double m = plot_get_min_x(c->plots->data);
        for (i = 1; i < c->n_plots; i++)
        {
            plot * p = chart_get_plot(c, i);
            if (p->n > 0)
            {
                double v = plot_get_max_x(p);
                if (v > M)
                    M = v;
                v = plot_get_min_x(p);
                if (v < m)
                    m = v;
            }
        }
        if (c->x_axis.mode == LINEAR)
            M += (M-m)*0.05;
        return M;
    }
    return c->x_axis.range_max;
}

double chart_get_min_x(chart *c)
{
    if (c->x_axis.autoscale  == TRUE){
        if (!c->n_plots) 
            return 0;
        unsigned int i;
        double M = plot_get_max_x(c->plots->data);
        double m = plot_get_min_x(c->plots->data);
        for (i = 1; i < c->n_plots; i++)
        {
            plot * p = chart_get_plot(c, i);
            if (p->n > 0)
            {
                double v = plot_get_max_x(p);
                if (v > M)
                    M = v;
                v = plot_get_min_x(p);
                if (v < m)
                    m = v;
            }
        }
        if (c->x_axis.mode == LINEAR)
            m -= (M-m)*0.05;
        return m;
    }
    return c->x_axis.range_min;
}

double chart_get_max_y(chart *c)
{
    if (c->y_axis.autoscale == TRUE){
        if (!c->n_plots)
            return 0;
        unsigned int i;
        double M = plot_get_max_y(c->plots->data);
        double m = plot_get_min_y(c->plots->data);
        for (i = 1; i < c->n_plots; i++)
        {
            plot * p = chart_get_plot(c, i);
            if (p->n > 0)
            {
                double v = plot_get_max_y(p);
                if (v > M)
                    M = v;
                v = plot_get_min_y(p);
                if (v < m)
                    m = v;
            }
        }
        if (c->y_axis.mode == LINEAR)
            M += (M-m)*0.05;
        if (M == m)
            return M + 0.5;
        return M;
    }
    return c->y_axis.range_max;
}

double chart_get_min_y(chart *c)
{
    if (c->y_axis.autoscale == TRUE){
        if (!c->n_plots)
            return 0;
        unsigned int i;
        double M = plot_get_max_y(c->plots->data);
        double m = plot_get_min_y(c->plots->data);
        for (i = 1; i < c->n_plots; i++)
        {
            plot * p = chart_get_plot(c, i);
            if (p->n > 0)
            {
                double v = plot_get_max_y(p);
                if (v > M)
                    M = v;
                v = plot_get_min_y(p);
                if (v < m)
                    m = v;
            }
        }
        if (c->y_axis.mode == LINEAR)
            m -= (M-m)*0.05;
        if (M == m)
            return m - 0.5;
        return m;
    }
    return c->y_axis.range_min;
}


unsigned int 
chart_add_plot(chart* c, plot* p)
{
    clist_append(c->plots, p);
    return ++c->n_plots;
}

plot* 
chart_get_plot(chart* c, unsigned int i)
{
    return clist_data_at(c->plots, i);
}


void chart_free(chart *c)
{
    if (c->title)
        free(c->title);
    if (c->x_axis.label)
        free(c->x_axis.label);
    if (c->y_axis.label)
        free(c->y_axis.label);
    plot_list_free(c->plots);
    free(c);
}


void plot_list_free(clist *pl)
{
    if (!pl)
        return;
    plot_free(pl->data);
    plot_list_free(pl->next);
    free(pl);
}

void bar_pref_free(barPref * d)
{   
    if (!d)
        return;
    if (d->line_color)
        free(d->line_color);
    free(d);
}

void plot_free(plot *p)
{
    if (!p)
        return;
    if (p->type == BAR)
        bar_pref_free(p->extra_data);
    if (p->color)
        free(p->color);
    if (p->label)
        free(p->label);
    if (p->x_data)
        free(p->x_data);
    if (p->y_data)
        free(p->y_data);
    free(p);
}