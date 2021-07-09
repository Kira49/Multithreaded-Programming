#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define ll long long int
#define pmt pthread_mutex_t
#define punlock pthread_mutex_unlock
#define plock pthread_mutex_lock
#define pinit pthread_mutex_init
#define pdest	pthread_mutex_destroy
ll n, m, o;
ll total_students;
ll waiting_students;
ll company_batches1[50];
ll company_batches2[50];
ll company_vaccines[50];
ll company_id[50];
ll zone_slots[50];
ll slots_available[50];
float probability_arr[50];
static pmt company_mutex[50], zone_mutex[50], student_mutex, studentw_mutex;
typedef struct c
{
	ll id;
	float prob;
}c;
typedef struct zs
{
	ll id;
}zs;
ll min(ll a, ll b, ll c)
{
	if(a<=b)
	{
		if(a<=c)
		{
			return a;
		}
		return c;
	}
	if(b<=a)
	{
		if(b<=c)
		{
			return b;
		}
		return c;
	}
}
ll check_for_antibodies(ll id)
{
	srand(time(0));
	ll result=rand()%2;
	return result;

}
void* companythr_handler(void* inp)
{
	c* input=(c*) inp;
	ll id=input->id;
	float prob=input->prob;
	int fl=1;
	while(fl)
	{
		srand(time(0));
		ll sleep_time=rand()%4+2;
		plock(&company_mutex[id]);
		srand(time(0));
		int f1=1;
		company_batches1[id]=rand()%5+1, company_batches2[id]=company_batches1[id];
		sleep(sleep_time);
		printf("Pharmaceutical Company %lld is preparing %lld batches of vaccines which have success probability %f", id, company_batches1[id], prob);
		printf("\n\n");
		srand(time(0));
		company_vaccines[id]=rand()%11+10;
		printf("Pharmaceutical Company %lld has prepared %lld batches of vaccines which have success probability %f.\nWaiting for all the vaccines to be used to resume production", id, company_batches1[id], prob);
		printf("\n\n");
		punlock(&company_mutex[id]);
		while(f1)
		{
			//printf("loop1 entered infinite phase %lld\n", total_students);
			plock(&company_mutex[id]);
			if (company_batches2[id]==0)
			{
				punlock(&company_mutex[id]);
				f1=0;
				continue;
			}
			punlock(&company_mutex[id]);

			plock(&student_mutex);
			if (total_students==0)
			{
				punlock(&student_mutex);
				f1=0;
				continue;
			}
			punlock(&student_mutex);
		}
		plock(&student_mutex);
		if (total_students==0)
		{
			punlock(&student_mutex);
			fl=0;
			continue;
		}
		punlock(&student_mutex);
		printf("All the vaccines prepared by Pharmaceutical Company %lld are emptied. Resuming production now.", id);
		printf("\n\n");
	}
}
void* zonethr_handler(void* inp)
{
	zs*input=(zs*) inp;
	ll id=input->id;
	ll flag=1;
	while(flag)
	{
		//printf("loop2 entered infinite phase\n");
		float vaccine_prob;
		ll vaccines;
		while(flag)
		{
			//printf("loop3 entered infinite phase\n");
			// ll flag=0;
			for (ll i=0;i<n;i++)
			{
				plock(&company_mutex[i]);
				if (company_batches1[i]>0)
				{
					flag=0;
					printf("Pharmaceutical Company %lld is delivering a vaccine batch to Vaccination Zone %lld which has success probability %f", i, id, probability_arr[i]);
					printf("\n\n");
					company_id[id]=i, vaccines=company_vaccines[i];
					company_batches1[i]--;
					vaccine_prob=probability_arr[i];
					punlock(&company_mutex[i]);
					break;
				}
				punlock(&company_mutex[i]);

			}
		}
		printf("Pharmaceutical Company %lld has delivered vaccines to Vaccination zone %lld, resuming vaccinations now", company_id[id], id);
		printf("\n\n");
		ll flag=0;
		while(vaccines>0)
		{
			//printf("loop4 entered infinite phase\n");

			plock(&zone_mutex[id]);


			plock(&studentw_mutex);
			ll minval=min(8, waiting_students, vaccines);
			punlock(&studentw_mutex);

			//printf("bullshit %lld\n", id);
			srand(time(0));
			if(minval==0)
			{
				zone_slots[id]=1;
				slots_available[id]=1;
			}
			else
			{
				zone_slots[id]=rand()%minval+1;
				slots_available[id]=zone_slots[id];
			}
			printf("Vaccination Zone %lld is ready to vaccinate with %lld slots", id, zone_slots[id]);
			printf("\n\n");
		//	printf("loop5 entered infinite phase %lld %lld %lld\n", slots_available[id], waiting_students, zone_slots[id]);

			punlock(&zone_mutex[id]);
			ll fl1=1;
			while(fl1)
			{
				//printf("loop5 entered infinite phase %lld %lld %lld\n", slots_available[id], waiting_students, zone_slots[id]);
				plock(&zone_mutex[id]), plock(&studentw_mutex);
				if ((slots_available[id]==0 || waiting_students==0) && slots_available[id]-zone_slots[id]!=0)
				{
					fl1=0;
					punlock(&studentw_mutex);
					zone_slots[id]=0;
					printf("Vaccination Zone %lld entering Vaccination Phase", id);
					printf("\n\n");
					vaccines=vaccines-zone_slots[id]+slots_available[id];
					punlock(&zone_mutex[id]);
					continue;

				}
				punlock(&studentw_mutex);
				punlock(&zone_mutex[id]);


				plock(&student_mutex);
				if (total_students==0)
				{
					punlock(&student_mutex);
					fl1=0;
					continue;
				}
				punlock(&student_mutex);
			}
			if (total_students==0)
			{
				punlock(&student_mutex);
				break;
			}
			punlock(&student_mutex);

		}
		plock(&student_mutex);
		punlock(&student_mutex);
		if(total_students!=0)
		{
			plock(&company_mutex[company_id[id]]);
			company_batches2[company_id[id]]--;
			punlock(&company_mutex[company_id[id]]);
			printf("Vaccination Zone %lld has run out of vaccines", id);
			printf("\n\n");
		}
		else
		{
			break;
		}
	}

}
void* studthr_handler(void* inp)
{
	zs* input=(zs*) inp;
	ll id=input->id;
	ll zone_id, comp_id;
	for (ll i=0; i<3; i++)
	{
		printf("Student %lld has arrived for his %lld round of vaccination", id, i+1);
		printf("\n\n");
		printf("Student %lld is waiting to be allocated a slot on a Vaccination Zone", id);
		printf("\n\n");
		ll flag=1;
		while(flag)
		{
			ll i=0;
			while(i<m)
			{
				plock(&zone_mutex[i]);
				if (slots_available[i]>0)
				{
					printf("Student %lld assigned a slot on the Vaccination Zone %lld which has %lld slots and waiting to be vaccinated", id, i, slots_available[i]);
					printf("\n\n");
					slots_available[i]--;
					zone_id=i;
					punlock(&zone_mutex[i]);
					flag=0;
					plock(&studentw_mutex);
					waiting_students--;
					punlock(&studentw_mutex);

					break;
				}
				punlock(&zone_mutex[i]);
				i++;
			}
		}
		comp_id=company_id[zone_id];
		printf("Student %lld on Vaccination Zone %lld has been vaccinated which has success probability %f", id, zone_id, probability_arr[comp_id]);
		printf("\n\n");
		ll result=check_for_antibodies(id);
		if (result==1)
		{
			printf("Student %lld has tested ‘positive’ for antibodies.", id);
			printf("\n\n");
			break;
		}
		else
		{
			plock(&studentw_mutex);
			waiting_students++;
			punlock(&studentw_mutex);
			printf("Student %lld has tested ‘negative’ for antibodies.", id);
			printf("\n\n");
		}
	}
	plock(&student_mutex);
	total_students--;
	punlock(&student_mutex);
}

int main()
{
	ll i;
	scanf("%lld %lld %lld", &n, &m, &o);
	for (i=0; i<n; i++)
		scanf("%f", &probability_arr[i]);
	total_students=o;
	waiting_students=o;
	pinit (&student_mutex, NULL);

	c* compthr_input[n+1];

	for (i=0; i<n;i++)
		pinit (&company_mutex[i], NULL);
		pinit (&studentw_mutex, NULL);
	for (i=0; i<m;i++)
		pinit (&zone_mutex[i], NULL);


	pthread_t company_thread[n+1], zone_thread[m+1], student_thread[o+1];

	zs* zonethr_input[m+1];
	zs* studthr_input[o+1];


	for (i=0; i<n; i++)
		compthr_input[i]=(c*)malloc(sizeof(c));
	for (i=0; i<m; i++)
		zonethr_input[i]=(zs*)malloc(sizeof(zs));
	for (i=0; i<o; i++)
		studthr_input[i]=(zs*)malloc(sizeof(zs));


	for (i=0; i<n; i++)
	{
		compthr_input[i]->prob=probability_arr[i];
		compthr_input[i]->id=i;
	}
	for (i=0; i<m; i++)
		zonethr_input[i]->id=i;
	for (i=0; i<o; i++)
		studthr_input[i]->id=i;


	for (i=0; i<n; i++)
		pthread_create(&company_thread[i], NULL, companythr_handler, (void*)compthr_input[i]);
	for (i=0; i<m; i++)
		pthread_create(&zone_thread[i], NULL, zonethr_handler, (void*)zonethr_input[i]);
	for (i=0; i<o; i++)
		pthread_create(&student_thread[i], NULL, studthr_handler, (void*)studthr_input[i]);


	for (i=0; i<n; i++)
		pthread_join(company_thread[i], NULL);
	for (i=0; i<m; i++)
		pthread_join(zone_thread[i], NULL);
	for (i=0; i<o; i++)
		pthread_join(student_thread[i], NULL);


	for (i=0; i<n; i++)
		pdest(&company_mutex[i]);
	for (i=0; i<m; i++)
		pdest(&zone_mutex[i]);
	pdest(&student_mutex);

	printf("---------------------------------------------------------------\n");
	printf("Simulation over\n");
	printf("---------------------------------------------------------------\n");
}
