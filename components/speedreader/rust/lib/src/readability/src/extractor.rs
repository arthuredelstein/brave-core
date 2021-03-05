use dom;
use markup5ever_rcdom::RcDom;
use markup5ever_rcdom::SerializableHandle;
use html5ever::tendril::TendrilSink;
use html5ever::{parse_document, serialize};
use markup5ever_rcdom::RcDom;
use markup5ever_rcdom::SerializableHandle;
use scorer;
use scorer::{Candidate, Title};
use std::cell::Cell;
use std::collections::BTreeMap;
use std::collections::HashMap;
use std::default::Default;
use std::io::Read;
use std::path::Path;
use url::Url;

#[derive(Debug)]
pub struct Product {
    pub title: String,
    pub content: String,
}

pub fn extract<R>(input: &mut R, url: &Url) -> Result<Product, std::io::Error>
where
    R: Read,
{
    let mut dom = parse_document(RcDom::default(), Default::default())
        .from_utf8()
        .read_from(input)?;

    extract_dom(&mut dom, url, &HashMap::new())
}

pub fn preprocess<R>(input: &mut R) -> Result<Title, std::io::Error>
where
    R: Read,
{
    let mut dom = parse_document(RcDom::default(), Default::default())
        .from_utf8()
        .read_from(input)?;

    let mut title = Title::default();
    let handle = dom.document.clone();
    scorer::preprocess(&mut dom, handle, &mut title);
    Ok(title)
}

pub fn extract_dom<S: ::std::hash::BuildHasher>(
    mut dom: &mut RcDom,
    url: &Url,
    features: &HashMap<String, u32, S>,
) -> Result<Product, std::io::Error> {
    let mut title = Title::default();
    let mut candidates = BTreeMap::new();
    let mut nodes = BTreeMap::new();
    let handle = dom.document.clone();

    // extracts title (if it exists) pre-processes the DOM by removing script
    // tags, css, links
    scorer::preprocess(&mut dom, handle.clone(), &mut title);

    // now that the dom has been preprocessed, get the set of potential dom
    // candidates and their scoring. a candidate contains the node parent of the
    // dom tree branch and its score. in practice, this function will go through
    // the dom and populate `candidates` data structure
    scorer::find_candidates(
        &mut dom,
        Path::new("/"),
        handle.clone(),
        &mut candidates,
        &mut nodes,
    );

    if candidates.iter().count() == 0 {
        return Err(std::io::Error::new(
            std::io::ErrorKind::InvalidInput,
            "No candidates found.",
        ));
    }

    let mut id: &str = "/";

    // top candidate is the top scorer among the tree dom's candidates. this is
    // the subtree that will be considered for final rendering
    let mut top_candidate: &Candidate = &Candidate {
        node: handle,
        score: Cell::new(0.0),
    };

    // scores all candidate nodes
    for (i, c) in candidates.iter() {
        let score = c.score.get() * (1.0 - scorer::get_link_density(&c.node));
        c.score.set(score);
        if score <= top_candidate.score.get() {
            continue;
        }
        id = i;
        top_candidate = c;
    }

    let mut bytes = vec![];

    scorer::clean(
        &mut dom,
        Path::new(id),
        top_candidate.node.clone(),
        url,
        &title.title,
        features,
        &candidates,
    );

    // Our CSS formats based on id="article".
    dom::set_attr("id", "article", top_candidate.node.clone(), true);
    let serialize_opts = serialize::SerializeOpts {
        traversal_scope: serialize::TraversalScope::IncludeNode,
        ..Default::default()
    };

    let document: SerializableHandle = top_candidate.node.clone().into();
    serialize(&mut bytes, &document, serialize_opts)?;
    let content = String::from_utf8(bytes).unwrap_or_default();

    Ok(Product {
        title: title.title,
        content: content,
    })
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::io::Cursor;

    #[test]
    fn test_extract_title() {
        let data = r#"
        <!DOCTYPE html>
        <html>
          <head>
            <title>This is title</title>
          </head>
        </html>
        "#;
        let mut cursor = Cursor::new(data);
        let product = preprocess(&mut cursor).unwrap();
        assert_eq!(product.title, "This is title");
    }

    #[test]
    fn test_prefer_meta() {
        let data = r#"
        <head>
        <meta property="og:title" content="Raspberry Pi 3 - All-time bestselling computer in UK"/>
        <meta property="hi" content="test"/>
        <title>Raspberry Pi 3 - All-time bestselling computer in UK - SimplyFound</title>
        </head>
        "#;
        let mut cursor = Cursor::new(data);
        let product = preprocess(&mut cursor).unwrap();
        assert_eq!(
            product.title,
            "Raspberry Pi 3 - All-time bestselling computer in UK"
        );
    }
}
