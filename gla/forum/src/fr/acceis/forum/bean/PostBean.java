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
package fr.acceis.forum.bean;

import java.sql.Date;

/**
 * The Class PostBean.
 */
public class PostBean {

	/** The author. */
	private final String author;

	/** The edited. */
	private Date edited;

	/** The id. */
	private final int id;

	/** The parent thread. */
	private final int parent_thread;

	/** The post. */
	private String post;

	/** The posted. */
	private final Date posted;

	/** The title. */
	private String title;

	/**
	 * Instantiates a new post bean.
	 *
	 * @param posted
	 *            the posted
	 * @param edited
	 *            the edited
	 * @param title
	 *            the title
	 * @param post
	 *            the post
	 * @param author
	 *            the author
	 * @param id
	 *            the id
	 * @param parent_thread
	 *            the parent thread
	 */
	public PostBean(final Date posted, final Date edited, final String title, final String post, final String author,
			final int id, final int parent_thread) {
		super();
		this.posted = posted;
		this.edited = edited;
		this.title = title;
		this.post = post;
		this.author = author;
		this.id = id;
		this.parent_thread = parent_thread;
	}

	/**
	 * Gets the author.
	 *
	 * @return the author
	 */
	public String getAuthor() {
		return author;
	}

	/**
	 * Gets the edited.
	 *
	 * @return the edited
	 */
	public Date getEdited() {
		return edited;
	}

	/**
	 * Gets the id.
	 *
	 * @return the id
	 */
	public int getId() {
		return id;
	}

	/**
	 * Gets the parent thread.
	 *
	 * @return the parent thread
	 */
	public int getParent_thread() {
		return parent_thread;
	}

	/**
	 * Gets the post.
	 *
	 * @return the post
	 */
	public String getPost() {
		return post;
	}

	/**
	 * Gets the posted.
	 *
	 * @return the posted
	 */
	public Date getPosted() {
		return posted;
	}

	/**
	 * Gets the title.
	 *
	 * @return the title
	 */
	public String getTitle() {
		return title;
	}

	/**
	 * Sets the edited.
	 *
	 * @param edited
	 *            the new edited
	 */
	public void setEdited(final Date edited) {
		this.edited = edited;
	}

	/**
	 * Sets the post.
	 *
	 * @param post
	 *            the new post
	 */
	public void setPost(final String post) {
		this.post = post;
	}

	/**
	 * Sets the title.
	 *
	 * @param title
	 *            the new title
	 */
	public void setTitle(final String title) {
		this.title = title;
	}
}
