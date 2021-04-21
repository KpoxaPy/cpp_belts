#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Human {
public:
    Human(const string& type, const string& name) 
        : type_(type)
        , name_(name)
    {
    }

	virtual ~Human() = default;

    string Name() const {
        return name_;
    }

    string Type() const {
        return type_;
    }

    string Description() const {
        return type_ + ": " + name_;
    }

    virtual void Walk(const string& destination) const {
        cout << Description() << " walks to: " << destination << endl;
    }

    void VisitPlaces(const vector<string>& places) const {
        for (auto p : places) {
            Walk(p);
        }
    }

private:
    const string type_;
    const string name_;
};

class Student : public Human {
public:
    Student(const string& name, const string& favourite_song)
        : Human("Student", name)
        , favourite_song_(favourite_song)
    {
    }

    void Walk(const string& destination) const override {
        Human::Walk(destination);
        SingSong();
    }

    void Learn() const {
        cout << Description() << " learns" << endl;
    }

    void SingSong() const {
        cout << Description() << " sings a song: " << favourite_song_ << endl;
    }

private:
    const string favourite_song_;
};


class Teacher : public Human {
public:
    Teacher(const string& name, const string& subject)
        : Human("Teacher", name)
        , subject_(subject)
    {
    }

    void Teach() const {
        cout << Description() << " teaches: " << subject_ << endl;
    }

private:
    const string subject_;
};


class Policeman : public Human {
public:
    Policeman(const string& name)
        : Human("Policeman", name)
    {
    }

    void Check(const Human& subj) const {
        cout << Description() << " checks " << subj.Type() << ". " << subj.Type() << "'s name is: " << subj.Name() << endl;
    }
};

int main() {
    Teacher t("Jim", "Math");
    Student s("Ann", "We will rock you");
    Policeman p("Bob");

    t.VisitPlaces({"Moscow", "London"});
    p.Check(s);
    s.VisitPlaces({"Moscow", "London"});
    return 0;
}
