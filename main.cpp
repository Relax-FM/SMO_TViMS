#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iomanip>

using namespace std;
static float Now_Time = 0.0;
static float End_Time = 0.0;
const int a_form = 3742;
const int c_form = 191;
static int grmin1 = 0, grmin2 = 0, grmax = 0;

struct Transact
{
	int id;
	float Start_Time;//==Start_Queue
	float End_Queue;
	float End_Time;
};



class Queue
{
private:
	int id;          // Очередь какого девайса
	int qmax;        // Максимальное кол-во  очереди (Изначально 1)
	int qnow;        // Кол-во очереди сейчас
	Transact** q;    // Транзакты в очереди
	ofstream& F;
public:
	Queue(int id1, ofstream& F1) :id(id1), F(F1)
	{
		id = id1;
		qmax = 1;
		qnow = 0;
		q = new Transact * [qmax];
	}
	void add_place_queue(Transact*);// Увеличивает кол-во мест в очереди(не,ну а вдруг
	int get_qnow() { return qnow; };
	int get_qmax() { return qmax; };
	void add(Transact*); // Добавляет в очередь.
	Transact* delete_last(); // Освобождает последнее место в очереди и передает первого в девайс

};

void Queue::add(Transact* x)
{
	if (qnow == qmax) add_place_queue(x);
	else if (qnow < qmax)
	{
		q[qnow] = x;
		qnow++;
	}
	else if (qnow > qmax)
	{
		F << "Error 1" << endl;
		exit(0);
	}
}

void Queue::add_place_queue(Transact* x)
{
	qmax++;

	Transact** new_q = new Transact * [qmax];
	for (int i = 0; i < (qmax - 1); i++)
		new_q[i] = q[i];

	new_q[qnow] = x;
	delete[] q;

	qnow++;
	q = new_q;
}

Transact* Queue::delete_last()
{
	Transact* y;

	if (qnow > 0)
	{
		y = q[0];

		if (qnow > 1)
		{
			for (int i = 0; i < qnow - 1; i++)
				q[i] = q[(i + 1)];

		}
		qnow = qnow - 1;
		return y;
	}
	else
	{
		F << "Error3" << endl;
		exit(0);
	}


}


class Device
{
private:
	int id;      // Какой девайс
	bool check;
	int grmin_d = 0;   // Минимальное время обслуживания
	int grmax_d = 0;   // Максимальное время обслуживания
	Queue q;     // Очередь пренадлежащая девайсу
	Transact* x;
	Transact* y;
	ofstream& F;
public:
	Device(int id1, int grmin1, int grmax1, ofstream& F1) :id(id1), grmin_d(grmin1), grmax_d(grmax1), q(id1, F1), F(F1) { check = 0; y = NULL; };
	void add_queue(Transact*); //добавление в очередь
	void leave_device(); // Кикает обработаный транзакт
	void leave_queue(); // Дает время обработки транзакта и выносит из очереди в девайс
	int checkqueue() { return q.get_qnow(); };
	int get_qnow() { return q.get_qnow(); };
	float get_time_exit()
	{
		if (y != NULL) return (y->End_Time);
		else
		{
			F << "Error'" << endl;
			exit(-1);
		}
	};
	bool check_device();
	bool det_check() { return check; };
};

void Device::add_queue(Transact* x)
{
	F << setw(10) << left << setprecision(3) << fixed << x->Start_Time << right << setw(12) << "generate" << setw(12) << x->id << setw(10) << "-" << endl;
	F << setw(10) << left << setprecision(3) << fixed << x->Start_Time << right << setw(12) << "queue" << setw(12) << x->id << setw(10) << id << endl;
	q.add(x);
	if (check == 0) leave_queue();
}

void Device::leave_queue()
{
	y = q.delete_last();
	srand(time(0) + y->id);
	check = 1;
	y->End_Queue = Now_Time;
	F << setw(10) << left << setprecision(3) << fixed << y->End_Queue << right << setw(12) << "depart" << setw(12) << y->id << setw(10) << id << endl;
	y->End_Time = y->End_Queue + ((float)grmin_d) + (((float)(rand() % ((grmax - grmin_d) * 1000))) / ((float)(1000)));
	F << setw(10) << left << setprecision(3) << fixed << y->End_Queue << right << setw(12) << "advance" << setw(12) << y->id << setw(10) << id << setw(10) << " " << setw(10) << ((y->End_Time) - (y->End_Queue)) << endl;
}

void Device::leave_device()
{
	check = 0;
	F << setw(10) << left << setprecision(3) << fixed << y->End_Time << right << setw(12) << "reliase" << setw(12) << y->id << setw(10) << id << setw(10) << get_qnow() << endl;
	int i = get_qnow();
	if (i == 0) i = 0;
	else if (i > 0) i = i - 1;
	F << setw(10) << left << setprecision(3) << fixed << y->End_Time << right << setw(12) << "terminate" << setw(12) << y->id << setw(10) << id << setw(10) << i << endl;
	if (get_qnow() >= 1) leave_queue();


}

bool Device::check_device()
{
	if (check == 0) return 0;
	else return 1;
}


///////////////////////////////////////



///////////////////////////////////////

void print_start(ofstream& F)
{
	F << setw(10) << left << "Time" << right << setw(12) << "Action:" << setw(12) << "Transact №" << setw(10) << "Device №" << setw(10) << "Queue" << setw(10) << "Act_Time" << endl;
}

void print_generate(ofstream& F, Transact* tran)
{
	F << setw(10) << left << setprecision(3) << fixed << tran->Start_Time << right << setw(12) << "generate" << setw(12) << tran->id << setw(10) << "-" << endl;
}

bool check_queue(Device d1, Device d2)
{
	int q1 = 0, q2 = 0;
	q1 = d1.checkqueue();
	if (d1.det_check() == 1) q1 = q1 + 1;
	q2 = d2.checkqueue();
	if (d2.det_check() == 1) q2 = q2 + 1;
	if (q1 <= q2) return 1;
	else if (q2 < q1) return 0;
}


int main()
{
	int id = 1;
	bool check = 0;
	float Time_Transact = 0.0;

	srand(time(0));
	cout << "Please entry work time (seconds) : \n";
	cin >> End_Time;
	cout << "Please entry the minimum service time for first device :" << endl;
	cin >> grmin1;
	cout << "Please entry the minimum service time for second device :" << endl;
	cin >> grmin2;
	cout << "Please entry the maximum service time for device :" << endl;
	cin >> grmax;

	ofstream F, J;
	SYSTEMTIME st;

	Device dev1(1, grmin1, grmax, F), dev2(2, grmin2, grmax, F);

	J.open("Journal.txt", ios::out);
	F.open("log-file.txt", ios::out);

	GetLocalTime(&st);
	J << st.wDay << "/" << st.wMonth << "/" << st.wYear << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << " Model Translation Begun." << endl;
	system("TYPE Journal.txt");


	/*     */

	print_start(F);

	Transact* tran = new Transact
	{
		id,
		0,
		0,
		0
	};


	dev1.add_queue(tran);



	/*     */

	system("CLS");
	GetLocalTime(&st);
	J << st.wDay << "/" << st.wMonth << "/" << st.wYear << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << " Ready." << endl;
	system("TYPE Journal.txt");
	//Sleep(2500);

	system("CLS");
	GetLocalTime(&st);
	J << st.wDay << "/" << st.wMonth << "/" << st.wYear << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << " Simulation in Progress." << endl;
	system("TYPE Journal.txt");



	tran = new Transact
	{
		++id,
		Now_Time + (((float)(rand() % (grmax * 1000))) / ((float)(1000))),
		0,
		0
	};
	Time_Transact = tran->Start_Time;

	while (Now_Time <= End_Time)
	{
		if ((dev1.get_time_exit() < Time_Transact) && (dev2.check_device() == 0) && (dev1.get_time_exit() > Now_Time) && (dev1.check_device() == 1))
		{

			Now_Time = dev1.get_time_exit();
			if (Now_Time > End_Time) break;
			dev1.leave_device();

		}

		else if ((dev1.get_time_exit() > Time_Transact) && (dev2.check_device() == 0) && (Time_Transact > Now_Time) && (dev1.check_device() == 1))
		{
			Now_Time = Time_Transact;
			if (Now_Time > End_Time) break;
			if ((check = check_queue(dev1, dev2)) == 1)dev1.add_queue(tran);
			else if ((check = check_queue(dev1, dev2)) == 0)dev2.add_queue(tran);
			tran = new Transact
			{
				++id,
				Now_Time + (((float)(rand() % (grmax * 1000))) / ((float)(1000))),
				0,
				0
			};
			Time_Transact = tran->Start_Time;
		}

		else if ((dev2.get_time_exit() > Time_Transact) && (dev1.check_device() == 0) && (Time_Transact > Now_Time) && (dev2.check_device() == 1))
		{
			Now_Time = Time_Transact;
			if (Now_Time > End_Time) break;
			if ((check = check_queue(dev1, dev2)) == 1)dev1.add_queue(tran);
			else if ((check = check_queue(dev1, dev2)) == 0)dev2.add_queue(tran);
			tran = new Transact
			{
				++id,
				Now_Time + (((float)(rand() % (grmax * 1000))) / ((float)(1000))),
				0,
				0
			};
			Time_Transact = tran->Start_Time;
		}

		else if ((dev2.get_time_exit() < Time_Transact) && (dev1.check_device() == 0) && (dev2.get_time_exit() > Now_Time) && (dev2.check_device() == 1))
		{
			Now_Time = dev2.get_time_exit();
			if (Now_Time > End_Time) break;
			dev2.leave_device();
		}

		else if ((dev1.get_time_exit() < Time_Transact) && (dev1.get_time_exit() < dev2.get_time_exit()) && (dev1.get_time_exit() > Now_Time) && (dev1.check_device() == 1) && (dev2.check_device() == 1))
		{
			Now_Time = dev1.get_time_exit();
			if (Now_Time > End_Time) break;
			dev1.leave_device();
		}

		else if ((dev2.get_time_exit() < Time_Transact) && (dev2.get_time_exit() < dev1.get_time_exit()) && (dev2.get_time_exit() > Now_Time) && (dev1.check_device() == 1) && (dev2.check_device() == 1))
		{
			Now_Time = dev2.get_time_exit();
			if (Now_Time > End_Time) break;
			dev2.leave_device();
		}

		else if ((Time_Transact < dev1.get_time_exit()) && (Time_Transact < dev2.get_time_exit()) && (Time_Transact > Now_Time) && (dev1.check_device() == 1) && (dev2.check_device() == 1))
		{
			Now_Time = Time_Transact;
			if (Now_Time > End_Time) break;
			if ((check = check_queue(dev1, dev2)) == 1)dev1.add_queue(tran);
			else if ((check = check_queue(dev1, dev2)) == 0)dev2.add_queue(tran);
			tran = new Transact
			{
				++id,
				Now_Time + (((float)(rand() % (grmax * 1000))) / ((float)(1000))),
				0,
				0
			};
			Time_Transact = tran->Start_Time;
		}
	}



	system("CLS");
	GetLocalTime(&st);
	J << st.wDay << "/" << st.wMonth << "/" << st.wYear << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << " The Simulation has ended.  Clock is ";
	J.precision(3);
	J << fixed << End_Time << endl;
	system("TYPE Journal.txt");

	J.close();
	system("Journal.txt");
	F.close();
	system("log-file.txt");
	return(0);

}
