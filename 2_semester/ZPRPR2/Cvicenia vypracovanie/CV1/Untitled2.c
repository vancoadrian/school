/*#include <stdio.h>
#include <stdlib.h>


double tyzdenna_mzda();


double tyzdenna_mzda(double h_mzda, double hod)
{
    if(hod<=40)
        return(h_mzda*hod);
    else if(hod >40 && hod<=60)
        return(h_mzda*40+h_mzda*1.5*(hod-40));
    else return(h_mzda*40+h_mzda*1.5*20+h_mzda*2*(hod-60));

}


int main()
{
    int n, i;
    double h_mzda, hod,c=0;
    scanf("%d",&n);
    for(i=0;i<n;i++)
    {
        scanf("%lf %lf",&h_mzda ,&hod);
        printf("Hod. mzda: %.2lf SKK/hod, hodin: %.2lf, spolu: %.2lf SKK\n",h_mzda ,hod ,tyzdenna_mzda(h_mzda, hod));
        c+=tyzdenna_mzda(h_mzda, hod);

    }
    printf("Celkova mzda: %lf SKK\n",c);

    return 0;
}
*/
