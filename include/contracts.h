/**
 * Ce fichier propose des macros afin de simplifier et de formaliser l'écriture
 * de programmes C/C++ avec le paradigme de programmation par contrats.
 *
 * Copyleft 2010 - Julien Fontanet <julien.fontanet@isonoe.net>
 *
 * v1.5 - 2010-09-30
 * - Le code a été réorganisé pour plus de lisibilité.
 * - Trois nouvelles macros (« if_else_debug », « if_debug », « if_not_debug »)
 *   permettent d'écrire du code selon que le mode débogage soit activé ou non.
 * - La classe « CertifiedObject » a été supprimée car elle n'apportait rien
 *   et, de part ses méthodes virtuelles, obligeait toute classe dérivée à
 *   avoir une « table de méthodes virtuelles » (peut engendrer des problèmes
 *   de taille de code et de performance).
 * - La macro « debug_code » a été supprimée, elle est remplacée par
 *   « if_debug ».
 *
 * v1.4 - 2010-03-07
 * - Tout comme l'en-tête standard « assert.h » il est possible d'inclure
 *   plusieurs fois « contracts.h » en modifiant son comportement à l'aide des
 *   macros NDEBUG et EXDEBUG.
 * - La macro « assert_exception » produit maintenant des messages d'erreurs
 *   clairs en cas d'échec.
 *
 * v1.3 - 2010-02-23
 * - Nouvelle macro pour C++ : assert_exception qui assure qu'une exception d'un
 *   certain type est levée lorsqu'un bout de code est executé (utile pour les
 *   jeux de tests).
 *
 * v1.2 - 2010-02-09
 * - En mode debug (si NDEBUG n'est pas défini), si la macro EXDEBUG est définie
 *   alors macros ensures, requires et validate n'interrompent plus l'exécution
 *   de programme en cas d'erreur mais lèvent une exception de type
 *   ContractViolated ayant pour message la condition qui a échouée.
 * - debug_stmt a été renommé en debug_code.
 *
 * v1.1 - 2010-01-27
 * -  Un destructeur virtuel a été ajouté à la classe CertifiedObject.
 */

#ifdef __cplusplus
#  include <cassert>
#  include <cstddef>
#  include <cstdlib>
#  include <cstdio>
#  include <stdexcept>
#  include <string>
#else
#  include <assert.h>
#  include <stddef.h>
#  include <stdlib.h>
#  include <stdio.h>
#endif

#ifndef H_CONTRACTS
#define H_CONTRACTS

/**
 * Permet d'écrire du code qui ne sera exécuté que si l'on est en mode
 * débogage.
 *
 * Ce code peut-être utile pour des pré-conditions où des post-conditions.
 *
 * Exemple :
 *   if_debug(int solde = this->solde);
 *   ensures(this->solde = solde + montant);
 */
#define if_debug(code) if_else_debug(code, (void) 0)

/**
 * Permet d'écrire du code qui ne sera exécuté que si l'on n'est pas en mode
 * débogage.
 */
#define if_not_debug(code) if_else_debug((void) 0, code)

/**
 * Cette macro permet de spécifier une pré-condition, c'est-à-dire une condition
 * nécessaire pour que la fonction/méthode s'éxecute correctement.
 *
 * Exemple :
 *   requires(solde > 0);
 */
#define requires(exp) (CONTRACTS_ASSERT(exp))

/**
 * Cette macro permet de spécifier une post-condition, c'est-à-dire une
 * condition qui doit être vrai à la fin de la fonction/méthode.
 *
 * Exemple :
 *   ensures(solde > 0);
 */
#define ensures(exp) (CONTRACTS_ASSERT(exp))


#ifdef __cplusplus // Définitions supplémentaires pour le C++

/**
 * Quand la macro EXDEBUG est définie, les macros « validate », « ensures » et
 * « requires » n'arrêtent plus le programme mais lancent une exception de ce
 * type.
 */
class ContractViolated : public std::logic_error
{
public:
	/**
	 * Construit une intance de ContractViolated qui indique qu'un contrat dans
	 * le code n'a pas été respecté.
	 *
	 * @param what La condition qui n'a pas été respectée.
	 */
	explicit ContractViolated(const std::string &what) : logic_error(what) {}
};

/**
 * Cette macro permet de vérifier que les invariants de classe d'un objet sont
 * respectés.
 *
 * Pour fonctionner, l'objet doit posséder une méthode « isValid() » constante
 * (c'est-à-dire qui ne modifie pas l'objet) retournant la valeur « true » si
 * l'état de l'objet est cohérent, false sinon.
 *
 * Exemple :
 *   validate(*this);
 *
 * @param object L'objet à vérifier.
 */
#define validate(object) (CONTRACTS_ASSERT((object).isValid()))

/**
 * Cette macro assure que l'exécution du code 'code' lève une exception de type
 * 'ex'.
 *
 * Exemple :
 *   std:vector<int> v;
 *   assert_exception(v.at(2) = 3, std::out_of_range);
 *
 * @param code Le code à exécuter.
 * @param ex   Le type de l'exception qui doit être levée.
 */
#define assert_exception(code, ex) \
	if_debug( \
		if (true) \
			try { \
				code; \
				__contracts_assert_failed(__FILE__, __LINE__, \
				                          "No exceptions has been thrown " \
				                          "(expected: “" #ex "”)."); \
			} \
			catch (ex) {} \
			catch (...) { \
				__contracts_assert_failed(__FILE__, __LINE__ , \
				                          "An unexpected exception has been " \
				                          "thrown (expected: “" #ex "”)."); \
			} \
		else ((void) 0) \
	)

#endif // __cplusplus


static inline
void
__contracts_assert_failed(const char *file, size_t line, const char *msg)
{
	fprintf(stderr, "*** assertion failed at %s:%lu\n%s\n", file,
	        (unsigned long) line, msg);
	abort();
}
#endif // H_CONTRACTS

// contracts.h peut-être inclue plusieurs avec des options différentes (NDEBUG,
// EXDEBUG), il faut donc réinitialiser les macros qui peuvent avoir changées.
#ifdef if_else_debug
#  undef if_else_debug
#  undef CONTRACTS_ASSERT
#endif

#ifdef NDEBUG

/**
 * L'appel à cette macro est remplacé par le code « code_if » si le mode
 * débogage est activé, par le code « code_else » sinon.
 *
 * Exemple :
 *   printf("Le mode débogage est %s\n.", if_else_debug("activé", "désactivé"));
 */
#  define if_else_debug(code_if, code_else) code_else

#  define CONTRACTS_ASSERT(expr) (void) 0
#else
#  define if_else_debug(code_if, code_else) code_if
#  if defined(EXDEBUG) && defined(__cplusplus)
#    define CONTRACTS_ASSERT(expr) ((expr) ? (void) 0 : throw ContractViolated(#expr))
#  else
#    define CONTRACTS_ASSERT(expr) assert(expr)
#  endif
#endif // NDEBUG
