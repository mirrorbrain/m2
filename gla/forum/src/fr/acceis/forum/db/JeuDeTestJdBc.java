/*
 * The GNU General Public License does not permit incorporating your program
 * into proprietary programs. If your program is a subroutine library, you may
 * consider it more useful to permit linking proprietary applications with the
 * library. If this is what you want to do, use the GNU Lesser General Public
 * License instead of this License. But first, please read
 * <http://www.gnu.org/philosophy/why-not-lgpl.html>.
 * ----------------------------------------------------------------------------
 * This source file is part of a university project and belongs to its creator
 * and the university it was made for.
 * @author Maël Nogues mael.nogues@outlook.com
 */
package fr.acceis.forum.db;

import java.sql.Connection;
import java.sql.Statement;

/**
 * The Class JeuDeTestJdBc.
 */
public class JeuDeTestJdBc {

	/** The Constant QUERIES. */
	public final static String[] QUERIES = { "drop table users if exists", "drop table threads if exists",
			"drop table posts if exists",
			"create table users (id bigint not null, login varchar(255), password binary(64), mail varchar(255), joined date, posts bigint default 0, salt binary(64), primary key (id))",
			"create table threads (id bigint not null, title varchar(255), author varchar(255), created date, primary key (id))",
			"create table posts (id bigint not null, parent bigint not null, title varchar(255), author varchar(255), post varchar(510), posted date, edited date, primary key (id))" };

	/**
	 * The main method.
	 *
	 * @param args
	 *            the arguments
	 * @throws Exception
	 *             the exception
	 */
	public static void main(final String[] args) throws Exception {
		final Connection c = Connexion.connect();
		final Statement s = c.createStatement();

		for (final String query : QUERIES)
			s.executeUpdate(query);

		Utils.addUser("admin", "admin", "admin@forum.com");
		Utils.addUser("pierre", "pierre", "pierre@forum.com");
		Utils.addUser("paul", "paul", "paul@forum.com");
		Utils.addUser("jacques", "jacques", "jacques@forum.com");

		Utils.newThread("test", "coucou", "pierre");
		Utils.newThread("essai", "coucou", "paul");

		s.close();
		c.close();
	}

}
