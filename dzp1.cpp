#include <cstdlib>
#include <iostream>
using namespace std;

class Stablo {
private:
	struct Cvor {
		int x, zauzeto;
		Cvor* otac, *brat;// dodati pokazivaci na bracu da bi ispis level ordera bio efikasniji i da ne bi bilo potrebe za pravljenjem reda
		Cvor** sinovi;//uzimam pretpostavku da je koriscenje ovoga brze nego pokazivanje na najlevlji element(da otac pokazuje na najlevljeg sina) i lakse za implementiranje
		Cvor(int xx) {
			x = xx;
			zauzeto = 0;
			otac = brat = nullptr;
			sinovi = nullptr;
		}
		~Cvor() {
			delete[] sinovi;
			sinovi = nullptr;
		}
	};
	Cvor* koren, *posl;
	//Cvor* najlevljielement=nullptr; moglo je ovo da se jos uprosti
	void kopiraj(const Stablo& s);
	void premesti(Stablo& s);
	void obrisi();
	int m;
public:
	explicit Stablo(int mm = 2) {
		if (mm < 2) { cout << "Stablo ne sme biti stepena manjeg od 2!"; exit(1); }
		koren = posl = nullptr;
		m = mm;
	}
	~Stablo() { obrisi(); }
	Stablo(const Stablo& s) { kopiraj(s); }
	Stablo(Stablo&& s) { premesti(s); }
	Stablo& operator=(const Stablo& s) {
		if (this != &s) {
			obrisi();
			kopiraj(s);
		}
		return *this;
	}
	Stablo& operator=(Stablo&& s) {
		if (this != &s) {
			obrisi();
			premesti(s);
		}
		return *this;
	}
	Stablo& operator+=(int broj);
	void ispisilvl()const;
	friend int lvlvisina(const Stablo &s);
	friend ostream& operator<<(ostream& it, const Stablo& s);
};
void Stablo::kopiraj(const Stablo & s)
{
	Cvor *tekk = s.koren; Cvor* levi = nullptr;
	m = s.m;
	while (tekk != nullptr) {
		Cvor* novi = new Cvor(tekk->x);
		if (koren == nullptr) {
			koren = posl = novi;

		}
		else if (koren == posl) {
			if (posl->zauzeto == 0)posl->sinovi = new Cvor*[m];
			posl->sinovi[posl->zauzeto++] = novi;
			novi->otac = posl;
			posl = novi;//najlevljielement=novi; Ukoliko bih koristio najlevljielement pokazivac!
		}
		else if (koren != posl) {
			Cvor* tek = posl->otac; bool z = 0;
			if (tek->zauzeto == m) {
				if (tek->brat == nullptr) { Cvor* tekk = koren; while (tekk->zauzeto != 0) { tekk = tekk->sinovi[0]; }tek = tekk; z = 1; }//moze umesto while da je tek=najlevljielement;(ukoliko bi se while smatralo obilazenjem stabla)
				else if (tek->brat != nullptr) { tek = tek->brat; }
			}
			if (tek->zauzeto == 0)tek->sinovi = new Cvor*[m];
			tek->sinovi[tek->zauzeto++] = novi;
			if (z == 0) posl->brat = novi;
			posl = novi;
			novi->otac = tek;
		}
		if (tekk->brat == nullptr) {
			if (tekk == s.koren && tekk->zauzeto != 0) {
				tekk = tekk->sinovi[0]; if (tekk != nullptr) { levi = tekk->sinovi[0]; }
				else levi = nullptr;
			}
			else {
				tekk = levi;  if (tekk != nullptr && tekk->zauzeto != 0) { levi = tekk->sinovi[0]; }
				else levi = nullptr;
			}
		}
		else if (tekk->brat != nullptr) { tekk = tekk->brat; }
	}
}

void Stablo::premesti(Stablo & s)
{
	koren = s.koren;
	posl = s.posl;
	s.koren = s.posl = nullptr;
	m = s.m;
}

void Stablo::obrisi()
{
	Cvor *tek = koren, *stari = nullptr, *levi = nullptr, *pomocni = tek; bool nasao = 0;
	while (tek != nullptr) {
		nasao = 0;
		pomocni = tek;
		while (pomocni != nullptr && !nasao) {
			for (int i = 0; i < pomocni->zauzeto; i++) {
				if (pomocni->sinovi[i] != nullptr) { levi = pomocni->sinovi[i]; nasao = 1; break; }
			}
			pomocni = pomocni->brat;
		}
		if (nasao == 0)levi = nullptr;
		while (tek != nullptr) {
			stari = tek;
			tek = tek->brat;
			for (int j = 0; j < stari->zauzeto; j++)stari->sinovi[j] = nullptr;
			delete stari;
		}
		tek = levi;
	}
	koren = nullptr;
}

Stablo & Stablo::operator+=(int broj)
{
	Cvor* novi = new Cvor(broj);
	if (koren == nullptr) {
		koren = posl = novi;
	}
	else if (koren == posl) {//dodavanje prvog levog elementa(mora odvojeno posto se kasnije oslanjam na oca,a ovde otac ne postoji)
		if (posl->zauzeto == 0)posl->sinovi = new Cvor*[m];
		posl->sinovi[posl->zauzeto++] = novi;
		novi->otac = posl;
		posl = novi;//najlevljielement=novi; Ukoliko bih koristio najlevljielement pokazivac!
	}
	else if (koren != posl) {
		Cvor* tek = posl->otac; bool z = 0;
		if (tek->zauzeto == m) {
			if (tek->brat == nullptr) { z = 1; Cvor* tekk = koren; while (tekk->zauzeto != 0) { tekk = tekk->sinovi[0]; }tek = tekk; }//moze umesto while da je tek=najlevljielement;(ukoliko bi se while smatralo obilazenjem stabla)
			else if (tek->brat != nullptr) { tek = tek->brat; }
		}
		if (tek->zauzeto == 0)tek->sinovi = new Cvor*[m];
		tek->sinovi[tek->zauzeto++] = novi;
		if (z == 0) posl->brat = novi;//if (z==1)najlevljielement=novi; 
		posl = novi;
		novi->otac = tek;
	}
	return *this;
}

void Stablo::ispisilvl() const//ispisuje klasican level order
{
	Cvor *tek = koren, *levi = nullptr; bool nasao;
	cout << "LEVEL ORDER: ";
	while (tek != nullptr) {
		nasao = 0;
		if (tek->sinovi != nullptr) { levi = tek->sinovi[0]; nasao = 1; }
		if (nasao == 0)levi = nullptr;
		while (tek != nullptr) {
			cout << tek->x;
			tek = tek->brat;
		}
		tek = levi;
	}
}


ostream & operator<<(ostream & it, const Stablo & s)//ispisuje tako da vizuelno izgleda kao stablo
{
	Stablo::Cvor *tek = s.koren, *levi = nullptr; int i = 0, j = -1, g = 0, granica = 1;
	while (tek != nullptr) {
		if ((i == 0) && tek->zauzeto != 0)levi = tek->sinovi[0];
		else if ((i == 0) && tek->zauzeto == 0)levi = nullptr;
		it << tek->x;
		i++; j++;
		if (j == g) { it << " "; j = 0; }
		if (i == granica) { if (g != s.m) { g = granica; } granica *= s.m; i = 0; it << endl; }
		if (tek->brat != nullptr)tek = tek->brat;
		else tek = levi;
	}
	return it;
}

int lvlvisina(const Stablo & s)
{
	Stablo::Cvor *tek = s.koren, *levi = nullptr; bool nasao; int visina = -1;
	while (tek != nullptr) {
		nasao = 0;
		if (tek->sinovi != nullptr) { levi = tek->sinovi[0]; nasao = 1; }
		if (nasao == 0)levi = nullptr;
		while (tek != nullptr) {//nema potrebe za ovim,ali da bi level order obilazak bio ispostovan,ici cu level orderom kroz stablo
			tek = tek->brat;
		}
		visina++;
		tek = levi;
	}
	if (s.koren == nullptr)return 0;
	else return visina;
}

int main() {
	bool kraj = false; int m;
	cout << "Unesite n-arnost stabla: ";
	cin >> m;
	Stablo s(m);
	int napravio = 0, k, o, n;
	cout << endl;
	while (!kraj) {
		cout << endl <<
			"1)Dodavanje broja\n2)Dodavanje vise brojeva\n3)Ispisi stablo\n4)Level order visina?\n5)Ispisi level order?\n0)Prekini program\n";
		cin >> k;
		switch (k) {
		case 0:kraj = true; cout << ":("; break;
		case 1:cout << "Koji clan zelite dodati?  "; cin >> o; s += o; napravio++; break;
		case 2:cout << "Koliko clanova zelite da dodate? "; cin >> n;
			cout << endl;
			cout << "Ukucajte svih " << n << " clanova:";
			for (int i = 0; i < n; i++) {
				napravio++;
				cin >> o;
				s += o;
			}
			break;
		case 3:cout << endl << s << endl; break;
		case 4:if (napravio != 0) { cout << "Visina stabla koristeci level order obilazak je: "; cout << lvlvisina(s); }
			   else {
			cout << "STABLO JOS NIJE NAPRAVLJENO!";
		} break;
		case 5:s.ispisilvl(); break;
		default:cout << "POGRESAN UNOS!  Unesite broj ponovo: "; continue;
		}
	}
	return 0;
}

/*
class Stabloo {
private:
	struct Cvorr {
		int zauzeto, x;
		Cvorr** sinovi, *otac;
		Cvorr(int xx) {
			x = xx;
			sinovi = new Cvorr*[5];
			otac = nullptr;
			zauzeto = 0;
		}
		~Cvorr() {
			delete[]sinovi;
			sinovi = nullptr;
		}
	};
	struct Elem {
		Elem *sled;
		Cvorr* cvor;
		Elem(Cvorr* c = nullptr) {
			sled = nullptr;
			cvor = c;
		}
		~Elem() {
			sled = nullptr;
			cvor = nullptr;
		}
	};
	int m;
	Cvorr *koren;
	Elem* red, *prvi, *posl;
public:
	Stabloo(int mm) { if (mm < 2)exit(3); m = mm; koren = nullptr; red = prvi = posl = nullptr; }
	~Stabloo();
	Stabloo& dodaj(int broj);
	void ispisi();
	friend int lvlvisina(const Stabloo& s);
};
Stabloo::~Stabloo()
{
	Elem*stari = nullptr;
	while (red != nullptr) {
		stari = red;
		red = red->sled;
		delete stari->cvor;
		delete stari;
	}
}
int lvlvisina(const Stabloo & s)
{
Cvorr* tek=s.koren;int visina=-1;
while(tek){
visina++;
tek=tek->sinovi[0];
}
if (s.koren == nullptr)return 0;
else return visina;
}
Stabloo & Stabloo::dodaj(int broj)
{
	Cvorr* novi = new Cvorr(broj);
	if (koren == nullptr) {
		prvi = posl = red = new Elem(novi);
		koren = novi;
}
	else {
		if (prvi->cvor->zauzeto == m) {
			prvi = prvi->sled;
	}
		if (prvi->cvor->zauzeto != m) {
			Elem* novi2 = new Elem(novi);
			posl->sled = novi2;
			posl = novi2;
			prvi->cvor->sinovi[prvi->cvor->zauzeto++] = novi;}
	}
	return *this;
}

void Stabloo::ispisi()
{
	cout << "LEVEL ORDER JE: "; Elem* tek = red;
	while (tek != nullptr) {
		cout << tek->cvor->x;
		tek = tek->sled;
	}
}
int main() {
	int x; Stabloo s(3);
	cin >>x;
	for (int i = 0; i < x; i++) {
		s.dodaj(i);
	}
	s.ispisi();
	int y;
	cin >> y;
	return 0;
}
*/