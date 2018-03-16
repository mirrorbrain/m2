/*
 * The GNU General Public License does not permit incorporating your program
 * into proprietary programs. If your program is a subroutine library, you may
 * consider it more useful to permit linking proprietary applications with the
 * library. If this is what you want to do, use the GNU Lesser General Public
 * License instead of this License. But first, please read
 * <http://www.gnu.org/philosophy/why-not-lgpl.html>.
 * ----------------------------------------------------------------------------
 * This source file is part of a university project and belongs to the ISTIC
 * and its author.
 * @author Maël Nogues mael.nogues@etudiant.univ-rennes.fr
 */
package fr.acceis.forum.adm;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;

import fr.acceis.forum.db.Connexion;

/**
 * The Class Tools.
 */
public class Tools {

	/**
	 * Adds the user.
	 *
	 * @param login
	 *            the login
	 * @param password
	 *            the password
	 */
	public static void addUser(final String login, final String password, final String mail) {
		try {
			final Connection c = Connexion.connect();
			PreparedStatement s = c.prepareStatement("select max(id) from users");
			long id = 0;
			if (s.execute())
				id = s.getResultSet().getLong("id") + 1;
			else
				throw new SQLException("could not get the current max id");

			final String q = "insert into users values(?,?,?,?,?,?)";
			s = c.prepareStatement(q);

			s.setLong(1, id);
			s.setString(2, login);
			s.setString(3, password);
			s.setString(4, mail);
			s.setString(5, "current_date");
			s.setLong(6, 0);
			if (!s.execute())
				throw new SQLException("could not insert the new user");

			s.close();
			c.close();
		} catch (final SQLException | InstantiationException | IllegalAccessException | ClassNotFoundException e) {
			System.err.println("Error on user add :\t" + e.getMessage());
			e.printStackTrace();
		}
	}
}
