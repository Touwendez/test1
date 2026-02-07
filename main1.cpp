#pragma once

#include <afx.h>        // pour CString, CObject, etc.
#include <afxcoll.h>    // pour CObArray
#include "vehicule.h"   // classe Vehicule (fournie)

class Vendeur_VO : public CObject
{
private:
    CString Nom;
    CObArray garage; // contiendra des Vehicule* (pointeurs)

protected:
    int recherche_ref(const CString& ref);

public:
    Vendeur_VO(const CString& nom);
    virtual ~Vendeur_VO();

    void achat(Vehicule v);

    Vehicule* vente(const CString& ref);

    CString info_garage_at(int index) const;
};



#include "pch.h"          // enlève si ton projet n’utilise pas pch
#include "vendeur_VO.h"
#include <afx.h>          // CString, CTime
#include <iostream>

using namespace std;

// =========================
// 1) Constructeur
// =========================
Vendeur_VO::Vendeur_VO(const CString& nom)
{
    Nom = nom;
}

// =========================
// 2) Destructeur
// =========================
// Ici on libère tous les Vehicule* stockés dans garage
Vendeur_VO::~Vendeur_VO()
{
    for (int i = 0; i < garage.GetSize(); i++)
    {
        Vehicule* pv = (Vehicule*)garage.GetAt(i);
        delete pv; // libère la mémoire
    }
    garage.RemoveAll(); // vide le tableau
}

// =========================
// 3) recherche_ref (protégée)
// =========================
int Vendeur_VO::recherche_ref(const CString& ref)
{
    for (int i = 0; i < garage.GetSize(); i++)
    {
        Vehicule* pv = (Vehicule*)garage.GetAt(i);

        if (pv->get_ref() == ref)
            return i;
    }
    return -1;
}

// =========================
// 4) achat
// =========================
void Vendeur_VO::achat(Vehicule v)
{
    // On crée une copie dynamique (pointeur) car CObArray stocke des pointeurs
    Vehicule* pv = new Vehicule(v);

    // Récupérer date/heure actuelle
    CTime now = CTime::GetCurrentTime();

    // Construire la référence demandée
    // Format: Marque_Modele_NumSerie(x)_YYYYMMDD_HH:MM:SS
    CString ref;
    ref.Format(_T("%s_%s_NumSerie(%d)_%04d%02d%02d_%02d:%02d:%02d"),
        pv->get_Marque().GetString(),
        pv->get_Modele().GetString(),
        pv->get_Num_serie(),
        now.GetYear(), now.GetMonth(), now.GetDay(),
        now.GetHour(), now.GetMinute(), now.GetSecond()
    );

    // Donner la référence au véhicule
    pv->referencer(ref);

    // Ajouter au garage
    garage.Add(pv);
}

// =========================
// 5) vente
// =========================
Vehicule* Vendeur_VO::vente(const CString& ref)
{
    int idx = recherche_ref(ref);

    if (idx == -1)
        return NULL;

    Vehicule* pv = (Vehicule*)garage.GetAt(idx);

    // Retirer du garage (sans delete car on le renvoie au client)
    garage.RemoveAt(idx);

    return pv;
}

// =========================
// 6) info_garage_at
// =========================
CString Vendeur_VO::info_garage_at(int index) const
{
    if (index < 0 || index >= garage.GetSize())
        return _T("pas de vehicule");

    Vehicule* pv = (Vehicule*)garage.GetAt(index);
    return pv->get_ref();
}



#include "pch.h"        // enlève si pas de pch
#include <iostream>
#include "vendeur_VO.h"

using namespace std;

int main()
{
    // 1) créer 3 véhicules
    Vehicule v1(_T("Ford"), _T("Fiesta"));
    Vehicule v2(_T("Renault"), _T("Clio"));
    Vehicule v3(_T("Peugeot"), _T("208"));

    // 2) créer 1 vendeur
    Vendeur_VO vendeur(_T("Bonocaz"));

    // 3) le vendeur achète les véhicules
    vendeur.achat(v1);
    vendeur.achat(v2);
    vendeur.achat(v3);

    // 4) afficher les informations du 2e véhicule
    // Attention: "2e véhicule" => index 1
    CString ref2 = vendeur.info_garage_at(1);
    wcout << L"Ref du 2e vehicule: " << (LPCTSTR)ref2 << endl;

    // 5) le vendeur vend un véhicule (par exemple celui qu'on vient d'afficher)
    Vehicule* vendu = vendeur.vente(ref2);

    if (vendu != NULL)
    {
        wcout << L"Vendu: " << (LPCTSTR)vendu->get_ref() << endl;
        delete vendu; // important: on libère la mémoire
    }
    else
    {
        wcout << L"Vente impossible (vehicule introuvable)" << endl;
    }

    return 0;
}
