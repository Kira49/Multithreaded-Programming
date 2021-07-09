Run the program:
1. Run the command gcc -pthread -o vaccine vaccine.c
2. Run ./vaccine
3. Input values of number of pharmaceutical companies, vaccination zones, students. 
Then input the probabilities of the success rate of each vaccine from the pharmaceutical companies.


Variables
1. Structs
Structs for pharmaceutical companies,vaccination zones and students.
Company : Each company struct c stores details about company thread ID and probability of the success rate.
	typedef struct c
	{
		ll id;
		float prob;
	}c;

Zone and student: Each struct zs stores details about zone and student thread ID.
	typedef struct zs
	{
		ll id;
	}zs;

2. Mutex variables 
	#define pmt pthread_mutex_t
	static pmt company_mutex[50] : for company
	static pmt zone_mutex[50] : for vaccination zone
	static pmt student_mutex : for total students 
	static pmt studentw_mutex : for students who are waiting for slots

3. Arrays for company batches, company vaccines, company id, slots in vaccination zones,        probability of the success rate.
 company_batches2[] is a duplicate array for company_batches1[] and slots_available[] is a duplicate array for zone_slots[] to store the values.
	ll company_batches1[50];
	ll company_batches2[50];
	ll company_vaccines[50];
	ll company_id[50];
	ll zone_slots[50];
	ll slots_available[50];
	float probability_arr[50];

4. long long int
   total_students : to keep track of total number of students.
   waiting_students : to keep track of the number of students waiting at a time.

Fuctions
1. Main:
  -> Takes input for number of pharmaceutical companies, vaccination zones, students and probabilities of the success rate of each vaccine from the pharmaceutical companies.
  -> Initializes total_students and waiting_students to 0. Initializes the random variables, structs and mutexes.
  -> Waits fro all students to be vaccinated(waiting for all threads to join)
  -> Destroys mutexes and terminates the program.

2. Company Function:
void* companythr_handler(void* inp)
	->Company thread will run in an infinite loop until total students become 0 i.e, all
	  students are vaccinated. 
	->Locks the mutex to prepare the vaccine.
	->Sleeps for random amount of time to simulate preparation time.
	->Genarates id and number of batches per company for that id randomly.
	->Unlocks the mutex.
	->Runs an infinte loop to check for any batches and students. Locks the company mutex and
	  checks if there are any batches of vaccine are still left to use. If it's zero it unlocks the company mutex and breaks the loops. Similarly for students mutex. At the end, it locks the student mtex and if total_students is 0, it unlocks the mutex and breaks the infinte loop. 

3. Zone function:
void* zonethr_handler(void* inp)
	->Zone thread will run in an infinite loop until a company delivers a batch of vaccines
	->In the loop ,lock the company mutex for every i less than n and for every company it 
	  delivers a batch of vaccine to the zone and decreases the number of batches for that company by 1. 
	->Unlocks the company mutex and breaks the loop if company delivers a batch of vaccine.
	->If number of vaccines is greater than 0 i.e, there are vaccines that can be used to
	  vaccinate,we run a loop.
	-> In the loop, zone mutex and student mutex are locked.
	-> Sleeps for random amount of time to simulate vaccination time.
	-> It randomly gives values for slots in each zone according to the given condition.
	-> Unlocks the students mutex and zone mutex.
	-> Lock student mutex and zone mutex.
	-> It enters the vaccination phase if all slots are allocated or no student is waiting.
	-> In the loop when a avccination zone enters vaccination phase, it decreases the value of 
	   total number vaccines by the number of vaccines in that slot in that zone.
	-> Unlock the student and zone mutex.
	-> Lock the student mutex, if total_students is zero break the loop, unlock the mutex.
	-> Lock the company mutex and decrease the value number of batches in the company by 
	   1(decrement in duplicate array, since original array values is decreased in the company function).
	-> Unlock company mutex.

4. Student function:
void* studthr_handler(void* inp)
	->For every student there are at maximum 3 chances to get vaccinated.
	->In the while loop, for every zone, lock the zone mutex.
	->If slots available in that zone is greater than zero, student will get vaccinated and 
	  slots available,waiting students value will be decremented by 1.
	->Zone mutex is unlocked.
	->Check for the antibodies for the student who got vaccinated(call check_for_antibodies)
	  function.
	->If antibodies are present, student will not be vaccinated again and breaks the loop.If 
	  antibodies are not present, depending on the slots available, student gets vaccinated again.
	->Total students to be vaccinated will be decreased by 1 after a student is succesfully 
	  vaccinated.

5. To check for antibodies:
ll check_for_antibodies(ll id)
	->Randomly generates number of antibodies.

6. Minimum function : To find the minimum of three numbers.
